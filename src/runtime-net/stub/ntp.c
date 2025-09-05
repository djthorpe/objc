#include <runtime-net/ntp.h>

struct net_ntp_t {
  int unused;
};

net_ntp_t *net_ntp_init(net_ntp_callback_t callback, void *user_data) {
  (void)callback;
  (void)user_data;
  return 0; // NULL indicates failure / unsupported
}

bool net_ntp_valid(net_ntp_t *ntp) {
  (void)ntp;
  return false; // always invalid in stub
}

void net_ntp_finalize(net_ntp_t *ntp) { (void)ntp; }
