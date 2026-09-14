#include "solarimetria.hpp"
#include <cmath>

// -----------------------------------------------------------------------
// Construtor
// -----------------------------------------------------------------------

Solarimetria::Solarimetria(double latitude, double longitude, double fuso_horario_h)
    : _latitude(latitude)
    , _longitude(longitude)
    , _fuso_horario_h(fuso_horario_h)
    , _beta(BETA_REPOUSO)
    , _gamma(GAMMA_REPOUSO)
    , _sol_visivel(false)
{}

// -----------------------------------------------------------------------
// API pública
// -----------------------------------------------------------------------

void Solarimetria::atualizar(const struct tempo &t)
{
    struct tm local = {};
    // t.year é o ano com 2 dígitos desde 2000 (ex.: 26 = 2026), enquanto
    // tm_year é o ano desde 1900 — por isso soma-se 100, e não subtrai 1900.
    local.tm_year = t.year + 100;
    local.tm_mon  = t.month - 1;      // tm_mon: 0–11
    local.tm_mday = t.mday;
    local.tm_hour = t.hour;
    local.tm_min  = t.min;
    local.tm_sec  = t.sec;

    PosicaoSolar pos = calcPosicao(local);
    _sol_visivel = pos.acima_horizonte;

    if (!pos.acima_horizonte) {
        _beta  = BETA_REPOUSO;
        _gamma = GAMMA_REPOUSO;
        return;
    }

    bool flip = false;
    _gamma = calcGamma(pos.azimute, flip);
    _beta  = calcBeta (pos.elevacao, flip);
}

// -----------------------------------------------------------------------
// Helpers privados
// -----------------------------------------------------------------------

double Solarimetria::clamp(double v, double lo, double hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/**
 * Dias julianos desde J2000.0.
 * Fórmula de Meeus, cap. 7 – válida para 1901–2099.
 *
 * `local` é a hora civil informada (não UTC). A conversão para UTC é
 * feita aqui mesmo, subtraindo o fuso horário configurado: como o
 * cálculo do dia juliano é puramente aditivo (UT/24 soma fração de dia),
 * isso funciona corretamente mesmo quando o ajuste de fuso "empurra" a
 * hora para o dia civil anterior ou seguinte — não é preciso tocar em
 * tm_mday/tm_mon/tm_year.
 */
double Solarimetria::diasDesdeJ2000(const struct tm& local) const
{
    int Y = local.tm_year + 1900;
    int M = local.tm_mon  + 1;
    int D = local.tm_mday;

    // hora local -> UTC: UTC = local - fuso_horario_h
    double UT = (local.tm_hour + local.tm_min / 60.0 + local.tm_sec / 3600.0)
              - _fuso_horario_h;

    if (M <= 2) { Y -= 1; M += 12; }
    int A = Y / 100;
    int B = 2 - A + A / 4;
    double JD = static_cast<int>(365.25 * (Y + 4716))
              + static_cast<int>(30.6001 * (M + 1))
              + D + B - 1524.5
              + UT / 24.0;
    return JD - 2451545.0;
}

/**
 * Calcula elevação e azimute solar.
 * Azimute: 0=Norte, 90=Leste, 180=Sul, 270=Oeste.
 */
Solarimetria::PosicaoSolar Solarimetria::calcPosicao(const struct tm& local) const
{
    const double jd = diasDesdeJ2000(local);
    const double T  = jd / 36525.0;  // séculos julianos

    // Longitude eclíptica solar
    const double L0    = std::fmod(280.46646 + 36000.76983 * T, 360.0);
    const double M     = std::fmod(357.52911 + 35999.05029 * T, 360.0);
    const double M_rad = toRad(M);

    const double C = (1.914602 - 0.004817 * T - 0.000014 * T * T) * std::sin(M_rad)
                   + (0.019993 - 0.000101 * T) * std::sin(2.0 * M_rad)
                   +  0.000289                  * std::sin(3.0 * M_rad);

    const double lon_sol = L0 + C;
    const double lon_rad = toRad(lon_sol);

    // Obliquidade da eclíptica
    const double eps_rad = toRad(23.439291 - 0.013004 * T);

    // Ascensão reta e declinação
    double asc_reta = toDeg(std::atan2(std::cos(eps_rad) * std::sin(lon_rad),
                                        std::cos(lon_rad)));
    if (asc_reta < 0.0) asc_reta += 360.0;

    const double dec_rad = std::asin(std::sin(eps_rad) * std::sin(lon_rad));

    // Tempo Sideral de Greenwich → ângulo horário local
    double GMST = 280.46061837 + 360.98564736629 * jd + 0.000387933 * T * T;
    GMST = std::fmod(GMST, 360.0);
    if (GMST < 0.0) GMST += 360.0;

    double LMST = std::fmod(GMST + _longitude, 360.0);
    if (LMST < 0.0) LMST += 360.0;

    double H = LMST - asc_reta;
    if (H < -180.0) H += 360.0;
    if (H >  180.0) H -= 360.0;

    const double H_rad   = toRad(H);
    const double lat_rad = toRad(_latitude);

    // Elevação geométrica (sem refração)
    const double sin_elev = std::sin(lat_rad) * std::sin(dec_rad)
                           + std::cos(lat_rad) * std::cos(dec_rad) * std::cos(H_rad);
    const double elev_geom = toDeg(std::asin(sin_elev));

    // Azimute geográfico (0=Norte, 90=Leste, 180=Sul, 270=Oeste).
    // Usa consistentemente a elevação GEOMÉTRICA (pré-refração) tanto no
    // numerador quanto no denominador.
    double cos_az = (std::sin(dec_rad) - std::sin(lat_rad) * sin_elev)
                  / (std::cos(lat_rad) * std::cos(toRad(elev_geom)));
    cos_az = clamp(cos_az, -1.0, 1.0);

    double az = toDeg(std::acos(cos_az));
    if (H > 0.0) az = 360.0 - az; // sol no Oeste → az > 180°

    // Correção de refração atmosférica de Bennet, aplicada por último
    // (só na elevação usada por beta — não afeta o azimute).
    double elev = elev_geom;
    if (elev > -1.5 && elev < 85.0) {
        const double refr = 1.02 / std::tan(toRad(elev + 10.3 / (elev + 5.11)));
        elev += refr / 60.0;
    }

    return { elev, az, elev >= ELEV_MINIMA };
}

/**
 * Calcula gamma (orientação do servo de azimute) e decide o `flip`.
 *
 * O eixo de gamma só gira 180°, ou seja, ele representa uma RETA (não
 * uma direção única): para um gamma fixo, a placa pode apontar tanto
 * para "gamma-90" (flip=false) quanto para "gamma+90" (flip=true,
 * direção oposta na mesma reta) — a escolha de qual lado usar fica a
 * cargo de calcBeta (ver abaixo).
 *
 * Dado o azimute solar (compass, 0=Norte/90=Leste/180=Sul/270=Oeste),
 * convertemos para um azimute "assinado" em (-180,180], com 0=Norte,
 * positivo=Leste, negativo=Oeste. Se esse valor já está dentro de
 * [-90,90] (metade Norte do horizonte), usamos flip=false direto. Caso
 * contrário, usamos a direção oposta (que cai dentro de [-90,90]) com
 * flip=true — assim o tracker consegue alcançar qualquer azimute do
 * horizonte (incluindo o lado Sul), não só a metade voltada pro Norte.
 */
int Solarimetria::calcGamma(double azimute, bool& flip) const
{
    double az_assinado = (azimute <= 180.0) ? azimute : azimute - 360.0;

    double eixo;
    if (az_assinado > 90.0) {
        flip = true;
        eixo = az_assinado - 180.0;
    } else if (az_assinado < -90.0) {
        flip = true;
        eixo = az_assinado + 180.0;
    } else {
        flip = false;
        eixo = az_assinado;
    }

    double gamma = eixo + 90.0;
    return static_cast<int>(std::round(clamp(gamma, GAMMA_MIN, GAMMA_MAX)));
}

/**
 * Calcula beta (inclinação do servo, 90=horizontal).
 *
 * Sem flip: a placa inclina em direção a "gamma-90"; beta = elevação
 * solar (0..90), decrescendo de 90 conforme o sol se aproxima do
 * horizonte nesse lado.
 *
 * Com flip: a placa inclina em direção a "gamma+90" (lado oposto do
 * mesmo eixo); beta = 180 - elevação solar (90..180), "passando" dos
 * 90° para o outro lado.
 *
 * Em ambos os casos sin(beta) = sin(elevação), preservando a elevação;
 * o sinal de cos(beta) (>90 ou <90) é que determina qual dos dois lados
 * do eixo de gamma a placa está usando.
 */
int Solarimetria::calcBeta(double elevacao, bool flip) const
{
    const double beta = flip ? (180.0 - elevacao) : elevacao;
    return static_cast<int>(std::round(clamp(beta, BETA_MIN, BETA_MAX)));
}
