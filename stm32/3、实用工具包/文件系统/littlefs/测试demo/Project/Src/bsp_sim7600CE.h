
#ifndef _BSP_SIM7600CE_H
#define _BSP_SIM7600CE_H











#ifdef _cplusplus
extern "C" {
#endif




void bsp_sim7600_init(void);
int8_t sim7600ce_send(uint8_t *s, uint8_t len);
int8_t sim7600ce_recv(uint8_t *recv, uint8_t buffLen);
int8_t sim7600ce_isPwrUp(void);
int8_t sim7600ce_run(uint8_t num);

#ifdef _cplusplus
}
#endif //_cplusplus

#endif
