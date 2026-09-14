/**
 * Solarimetria.hpp
 *
 * Classe de solarimetria para tracker solar de dois eixos.
 *
 * Convenções (tracker fisicamente montado voltado para o Norte):
 *   gamma (orientação) : 0 = Oeste, 90 = Norte, 180 = Leste
 *                         faixa mecânica [GAMMA_MIN, GAMMA_MAX] = [0,180]
 *                         (o eixo de gamma só GIRA 180°, então ele
 *                         representa uma RETA, não um raio — ela aponta
 *                         tanto para a direção "gamma-90" quanto para a
 *                         oposta "gamma+90")
 *   beta  (inclinação) : 90 = placa horizontal (voltada para o céu)
 *                         < 90 = inclina em direção a "gamma-90"
 *                         > 90 = inclina em direção a "gamma+90"
 *                         (a placa "passa" dos 90° pra usar o lado oposto
 *                         do eixo de gamma e cobrir os 360° do horizonte)
 *                         faixa mecânica [BETA_MIN, BETA_MAX] = [30,150]
 *
 * Fuso horário: atualizar() recebe a hora LOCAL (civil) do local de
 * instalação. A classe converte internamente para UTC usando o offset
 * fixo informado no construtor (padrão: -3h, horário de Brasília, sem
 * horário de verão desde 2019). Se o tracker for instalado em outro
 * fuso, ou se o fuso de instalação voltar a ter horário de verão,
 * ajuste o parâmetro `fuso_horario_h` do construtor.
 *
 * Uso:
 *   Solarimetria solarimetria(latitude, longitude); // fuso -3h (Brasília) por padrão
 *   solarimetria.atualizar(t);                       // t = hora LOCAL
 *   servo_beta.write(solarimetria.getBeta());
 *   servo_gamma.write(solarimetria.getGamma());
 */

#pragma once

#include <ctime>
#include <rtc.hpp>

class Solarimetria {
private:
    double _latitude;
    double _longitude;
    double _fuso_horario_h; // offset do fuso local em relação ao UTC, em horas (Brasília = -3)

    int  _beta;
    int  _gamma;
    bool _sol_visivel;

    // Constantes internas
    static constexpr double PI            = 3.14159265358979323846;
    static constexpr double ELEV_MINIMA   = -0.833; // compensação de refração

    // Helpers
    static double toRad(double graus) { return graus * PI / 180.0; }
    static double toDeg(double rad)   { return rad * 180.0 / PI;   }
    static double clamp(double v, double lo, double hi);

    struct PosicaoSolar {
        double elevacao;
        double azimute;
        bool   acima_horizonte;
    };

    double       diasDesdeJ2000(const struct tm& local) const;
    PosicaoSolar calcPosicao(const struct tm& local)    const;
    int          calcBeta (double elevacao, bool flip)   const;
    int          calcGamma(double azimute, bool& flip)   const;

public:
    // Limites dos servos
    static constexpr int BETA_MIN  = 30;
    static constexpr int BETA_MAX  = 150;
    static constexpr int GAMMA_MIN = 0;
    static constexpr int GAMMA_MAX = 180;

    // Posição de repouso (sol abaixo do horizonte): plano, voltado para o Norte
    static constexpr int BETA_REPOUSO  = 90;
    static constexpr int GAMMA_REPOUSO = 90;

    // Fuso horário padrão de instalação (Brasília, UTC-3, sem DST)
    static constexpr double FUSO_HORARIO_PADRAO_H = -3.0;

    /**
     * @param latitude        Latitude em graus decimais  (+Norte / −Sul)
     * @param longitude       Longitude em graus decimais (+Leste / −Oeste)
     * @param fuso_horario_h  Offset do fuso horário local em relação ao UTC,
     *                        em horas (ex.: -3 para horário de Brasília).
     */
    Solarimetria(double latitude, double longitude,
                 double fuso_horario_h = FUSO_HORARIO_PADRAO_H);

    /**
     * Atualiza o tempo e recalcula beta e gamma de acordo com a posição
     * solar (latitude/longitude/fuso configurados e o instante `t`).
     * Deve ser chamado a cada ciclo de controle (ex: a cada minuto).
     * O tempo deve ser fornecido em hora LOCAL (a conversão para UTC é
     * feita internamente usando o fuso configurado no construtor).
     */
    void atualizar(const struct tempo &t);

    /** Retorna o ângulo de inclinação para o servo [BETA_MIN, BETA_MAX]. */
    int getBeta()  const { return _beta;  }

    /** Retorna o ângulo de orientação para o servo [GAMMA_MIN, GAMMA_MAX]. */
    int getGamma() const { return 149 - _gamma; }

    /** Retorna true se o sol está acima do horizonte. */
    bool solVisivel() const { return _sol_visivel; }
};
