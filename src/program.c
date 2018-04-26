#include <time.h>
#include <syslog.h>
#include <stdlib.h>

#define BOMBA            20
#define ACTIVAR          HIGH
#define DESACTIVAR       LOW
#define TIEMPO_REGADO    (60)
#define INTERVALO_OK	 (600)

static time_t last_time;
static time_t last_ok_time;
static int bomba_activa = 0;

static int horas_riego[] = {8, 17, 18, 19, 20, -1};

int es_hora(void) {
	int rv = 0;
	struct tm hf;
	time_t t;
	int i;
	t = time(NULL);
	hf = *localtime(&t);
	i = 0;
	while (horas_riego[i] >= 0) {
		if ((hf.tm_hour == horas_riego[i]) && (hf.tm_min == 0)) {
			rv = 1;
			break;
		}
		i++;
	}
	return rv;
}

void setup(void)
{
	pinMode(21, OUTPUT);
	pinMode(BOMBA, OUTPUT);
	digitalWrite(BOMBA, DESACTIVAR);
	last_time = 0;
}

void loop(void)
{
	if (es_hora() && (!bomba_activa)) {
		bomba_activa = 1;
		digitalWrite(BOMBA, ACTIVAR);
		last_time = time(NULL);
		syslog(LOG_INFO, "regando");
	}

	if (bomba_activa) {
		if (time(NULL) >= (last_time+TIEMPO_REGADO)) {
			digitalWrite(BOMBA, DESACTIVAR);
			bomba_activa = 0;
			syslog(LOG_INFO, "fin riego");
		}
		digitalWrite(21, HIGH);
		delay(500);
		digitalWrite(21, LOW);
	}

	if (time(NULL) >= (last_ok_time+INTERVALO_OK)) {
		syslog(LOG_INFO, "regador ok");
		last_ok_time = time(NULL);
	}

	delay(500);
}
