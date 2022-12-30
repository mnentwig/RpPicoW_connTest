#include <string.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "dhcpserver.h"

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

void sendMsg(const ip_addr_t* senderAddr){
#if 0
  static struct udp_pcb *sendMsg_pcb = NULL;
  static struct pbuf* sendMsg_p = NULL;

  // === startup ===
  if (!sendMsg_p){
    const size_t nData = 16;    
    sendMsg_p = pbuf_alloc(PBUF_TRANSPORT, nData, PBUF_RAM);
    if (!sendMsg_p) blink(3);
  }

  if (sendMsg_pcb == NULL){
    sendMsg_pcb = udp_new();
    if (!sendMsg_pcb) blink(3);
    if (ERR_OK != udp_bind(sendMsg_pcb, IP_ADDR_ANY, /*any*/0)) blink(4);
  }

  // === copy data ===
  uint8_t* pData = (uint8_t*)sendMsg_p->payload;
  for (size_t ix = 0; ix < nData; ++ix)
    *(pData++) = 0;

  // === send reply ===
  //  if (ERR_OK != udp_connect(sendMsg_pcb, senderAddr, 8081)) blink(5);
  //  if (ERR_OK != udp_send(sendMsg_pcb, sendMsg_p)) blink(6);
  int err = udp_sendto(sendMsg_pcb, sendMsg_p, senderAddr, 8081);
  if (err != ERR_OK)
    blink(-err);
#else
  struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT,16,PBUF_RAM);
  //  memcpy(p->payload, Test, sizeof(Test));
    
  struct udp_pcb* pcb = udp_new();
  udp_bind(pcb, IP_ADDR_ANY, 8080);
  udp_connect(pcb, senderAddr, 8081);
  int err=udp_send(pcb,p);
  if(err!=ERR_OK) blink(-err);
  pbuf_free(p);
  udp_remove(pcb);
#endif
}

static volatile int deltaTime = 1000;
static void cb_udp_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *src_addr, u16_t src_port) {
  (void)arg;
  (void)upcb;
  (void)src_port;
  sendMsg(src_addr);
  deltaTime = 100;
  pbuf_free(p);
}

int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    const char *ap_name = "picow_test";
    const char *password = "buZZword";

    cyw43_arch_enable_ap_mode(ap_name, password, CYW43_AUTH_WPA2_AES_PSK);

    ip4_addr_t gw, mask;
    IP4_ADDR(&gw, 192, 168, 4, 1);
    IP4_ADDR(&mask, 255, 255, 255, 0);

    // === dhcp server (connect doesn't work otherwise) ===
    dhcp_server_t dhcp_server;
    dhcp_server_init(&dhcp_server, &gw, &mask);

    // === set up receive ===
    udp_init();
    struct udp_pcb *udp = udp_new();
    if (!udp) blink(9);
    if (ERR_OK != udp_bind( udp, IP_ADDR_ANY, /*port*/8080 )) blink(10);
    udp_recv(udp, cb_udp_recv, (void *)NULL);

    while(true) {
        static absolute_time_t led_time;
        static int led_on = true;

        // Invert the led
        if (absolute_time_diff_us(get_absolute_time(), led_time) < 0) {
            led_on = !led_on;
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
            led_time = make_timeout_time_ms(deltaTime);
        }
        cyw43_arch_poll();
        sleep_ms(1);
    }

    return 0;
}
