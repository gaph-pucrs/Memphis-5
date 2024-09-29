/**
 * Memphis-V
 * @file main.c
 *
 * @author Angelo Elias Dal Zotto (angelo.dalzotto@edu.pucrs.br)
 * GAPH - Hardware Design Support Group (https://corfu.pucrs.br/)
 * PUCRS - Pontifical Catholic University of Rio Grande do Sul (http://pucrs.br/)
 * 
 * @date September 2024
 * 
 * @brief SAFE (Security Anomaly Forecasting Engine) -- audio_video model
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include <memphis.h>
#include <memphis/monitor.h>
#include <memphis/services.h>
#include <memphis/oda.h>

#include "model.h"

int main()
{
	printf("SAFE audio_video model started at %d\n", memphis_get_tick());

	int num_inf = 0;
	while (true) {
		static uint32_t msg[7];
		memphis_receive_any(msg, sizeof(msg));
		switch (msg[0]) {
			case SEC_SAFE_REQ_APP:
				uint32_t ans[] = {SEC_SAFE_APP_RESP, SAFE_HASH_audio_video};
				memphis_send(ans, sizeof(ans), msg[1]);
				break;
			case SEC_INFER:
				num_inf++;
				bool prod_0 = false;
				bool prod_1 = false;
				bool prod_2 = false;
				bool prod_3 = false;
				bool prod_4 = false;
				bool prod_6 = false;
				bool cons_0 = false;
				bool cons_1 = false;
				bool cons_2 = false;
				bool cons_3 = false;
				bool cons_4 = false;
				bool cons_5 = false;
				switch (msg[4]) {
					case 0:
						prod_0 = true;
						break;
					case 1:
						prod_1 = true;
						break;
					case 2:
						prod_2 = true;
						break;
					case 3:
						prod_3 = true;
						break;
					case 4:
						prod_4 = true;
						break;
					case 6:
						prod_6 = true;
						break;
					default:
						break;
				}
				switch (msg[5]) {
					case 0:
						cons_0 = true;
						break;
					case 1:
						cons_1 = true;
						break;
					case 2:
						cons_2 = true;
						break;
					case 3:
						cons_3 = true;
						break;
					case 4:
						cons_4 = true;
						break;
					case 5:
						cons_5 = true;
						break;
					default:
						break;
				}
				// unsigned then = memphis_get_tick();
				float timestamp = msg[1] / 100000.0;
				float real_latency = msg[6] / 100.0;
				float pred_latency = score(
					timestamp, 
					msg[3], 
					msg[2], 
					prod_0, 
					prod_1, 
					prod_2, 
					prod_3, 
					prod_4, 
					prod_6, 
					cons_0, 
					cons_1, 
					cons_2, 
					cons_3, 
					cons_4, 
					cons_5
				);
				float diff = fabs(real_latency - pred_latency) / pred_latency;
				bool anom = diff > 0.05;
				// unsigned now = memphis_get_tick();
				// printf("Inference in %u us\n", (now - then)/100);
				// printf("Instance %f,%lu,%lu,%d,%d,%f,%f\n", timestamp, msg[3], msg[2], (int)msg[4], (int)msg[5], real_latency, pred_latency);
				// printf("Diff %.2f%%\n", diff);
				if (anom) {
					printf(
						"Anomaly @%lu %d->%d\n", 
						msg[1], 
						(int)msg[4], 
						(int)msg[5]
					);
				}
				break;
			case TERMINATE_ODA:
				printf("Exiting after %d inferences\n", num_inf);
				return 0;
			default:
				break;
		}
	}

	return 0;
}
