#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"

static const int port = WIFI_PORT; // CMakeLists.txt
void blink(int n){
  while (true){
    for (int ix = 0; ix < n; ++ix){
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
      sleep_ms(250);
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
      sleep_ms(250);
    }
    sleep_ms(500);
  }
}

static volatile bool received = false;
static void cb_udp_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *src_addr, u16_t src_port) {
  (void)arg;
  (void)upcb;
  (void)src_port;
  (void)src_addr;
  pbuf_free(p);
  //  blink(11); // success!
  received = true;
}

void sendMsg(const ip_addr_t* remoteAddr){
  struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, 16, PBUF_RAM);
  //  memcpy(p->payload, Test, sizeof(Test));
    
  struct udp_pcb* pcb = udp_new();
  udp_bind(pcb, IP_ADDR_ANY, port);
  udp_connect(pcb, remoteAddr, port);
  int err=udp_send(pcb,p);
  if(err!=ERR_OK) blink(-err);
  pbuf_free(p);
  udp_remove(pcb); // don't reuse ('alloc' appears to 'initialize'?)
}

int main() {
    stdio_init_all();

    ip4_addr_t remoteAddr;
    IP4_ADDR(&remoteAddr, 192,168,4,1);
    
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_GERMANY)) blink(1);
    cyw43_arch_enable_sta_mode();
    const char *ap_name = WIFI_SSID; // CMakeLists.txt
    const char *password = WIFI_PASSWORD; // CMakeLists.txt
    if (cyw43_arch_wifi_connect_timeout_ms(ap_name, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) 
      blink(2);
        
    // === set up receive ===
    udp_init();
    struct udp_pcb *udp = udp_new();
    if (!udp) blink(9);
    if (ERR_OK != udp_bind( udp, IP_ADDR_ANY, port )) blink(10);
    udp_recv(udp, cb_udp_recv, (void *)NULL);

    absolute_time_t tick = 0;
    while (true){
      received = false;
      sendMsg(&remoteAddr);
      sleep_ms(150);
      cyw43_arch_poll();
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, received);

      // link status times out after a few seconds. Todo: Reconnect attempt here
      if (cyw43_tcpip_link_status(&cyw43_state,CYW43_ITF_STA) != CYW43_LINK_UP)
	blink(7);
    }
    
    return 0;
}
