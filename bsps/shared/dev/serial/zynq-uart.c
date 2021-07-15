/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2013, 2017 embedded brains GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <dev/serial/zynq-uart.h>
#include <dev/serial/zynq-uart-regs.h>
#include <bsp/irq.h>

#include <bspopts.h>

#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS
static void zynq_uart_interrupt(void *arg)
{
  rtems_termios_tty *tty = arg;
  zynq_uart_context *ctx = rtems_termios_get_device_context(tty);
  volatile zynq_uart *regs = ctx->regs;
  uint32_t channel_sts;

  if ((regs->irq_sts & (ZYNQ_UART_TIMEOUT | ZYNQ_UART_RTRIG)) != 0) {
    regs->irq_sts = ZYNQ_UART_TIMEOUT | ZYNQ_UART_RTRIG;

    do {
      char c = (char) ZYNQ_UART_TX_RX_FIFO_FIFO_GET(regs->tx_rx_fifo);

      rtems_termios_enqueue_raw_characters(tty, &c, 1);

      channel_sts = regs->channel_sts;
    } while ((channel_sts & ZYNQ_UART_CHANNEL_STS_REMPTY) == 0);
  } else {
    channel_sts = regs->channel_sts;
  }

  if (ctx->transmitting && (channel_sts & ZYNQ_UART_CHANNEL_STS_TEMPTY) != 0) {
    rtems_termios_dequeue_characters(tty, 1);
  }
}
#endif

static bool zynq_uart_first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS
  zynq_uart_context *ctx = (zynq_uart_context *) base;
  volatile zynq_uart *regs = ctx->regs;
  rtems_status_code sc;
#endif

  rtems_termios_set_initial_baud(tty, ZYNQ_UART_DEFAULT_BAUD);
  zynq_uart_initialize(base);

#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS
  regs->rx_timeout = 32;
  regs->rx_fifo_trg_lvl = ZYNQ_UART_FIFO_DEPTH / 2;
  regs->irq_dis = 0xffffffff;
  regs->irq_sts = 0xffffffff;
  regs->irq_en = ZYNQ_UART_RTRIG | ZYNQ_UART_TIMEOUT;
  sc = rtems_interrupt_handler_install(
    ctx->irq,
    "UART",
    RTEMS_INTERRUPT_SHARED,
    zynq_uart_interrupt,
    tty
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return false;
  }
#endif

  return true;
}

#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS
static void zynq_uart_last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  zynq_uart_context *ctx = (zynq_uart_context *) base;

  rtems_interrupt_handler_remove(ctx->irq, zynq_uart_interrupt, tty);
}
#endif

static void zynq_uart_write_support(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS
  zynq_uart_context *ctx = (zynq_uart_context *) base;
  volatile zynq_uart *regs = ctx->regs;

  if (len > 0) {
    ctx->transmitting = true;
    regs->irq_sts = ZYNQ_UART_TEMPTY;
    regs->irq_en = ZYNQ_UART_TEMPTY;
    regs->tx_rx_fifo = ZYNQ_UART_TX_RX_FIFO_FIFO(buf[0]);
  } else {
    ctx->transmitting = false;
    regs->irq_dis = ZYNQ_UART_TEMPTY;
  }
#else
  ssize_t i;

  for (i = 0; i < len; ++i) {
    zynq_uart_write_polled(base, buf[i]);
  }
#endif
}

static bool zynq_uart_set_attributes(
  rtems_termios_device_context *context,
  const struct termios *term
)
{
  zynq_uart_context *ctx = (zynq_uart_context *) context;
  volatile zynq_uart *regs = ctx->regs;
  int32_t baud;
  uint32_t brgr = 0;
  uint32_t bauddiv = 0;
  uint32_t mode = 0;
  int rc;

  /*
   * Determine the baud rate
   */
  baud = rtems_termios_baud_to_number(term->c_ospeed);

  if (baud > 0) {
    rc = zynq_cal_baud_rate(baud, &brgr, &bauddiv, regs->mode);
    if (rc != 0)
      return rc;
  }

  /*
   * Configure the mode register
   */
  mode |= ZYNQ_UART_MODE_CHMODE(ZYNQ_UART_MODE_CHMODE_NORMAL);

  /*
   * Parity
   */
  mode |= ZYNQ_UART_MODE_PAR(ZYNQ_UART_MODE_PAR_NONE);
  if (term->c_cflag & PARENB) {
    if (!(term->c_cflag & PARODD)) {
      mode |= ZYNQ_UART_MODE_PAR(ZYNQ_UART_MODE_PAR_ODD);
    } else {
      mode |= ZYNQ_UART_MODE_PAR(ZYNQ_UART_MODE_PAR_EVEN);
    }
  }

  /*
   * Character Size
   */
  switch (term->c_cflag & CSIZE)
  {
  case CS5:
    return false;
  case CS6:
    mode |= ZYNQ_UART_MODE_CHRL(ZYNQ_UART_MODE_CHRL_6);
    break;
  case CS7:
    mode |= ZYNQ_UART_MODE_CHRL(ZYNQ_UART_MODE_CHRL_7);
    break;
  case CS8:
  default:
    mode |= ZYNQ_UART_MODE_CHRL(ZYNQ_UART_MODE_CHRL_8);
    break;
  }

  /*
   * Stop Bits
   */
  if (term->c_cflag & CSTOPB) {
    /* 2 stop bits */
    mode |= ZYNQ_UART_MODE_NBSTOP(ZYNQ_UART_MODE_NBSTOP_STOP_2);
  } else {
    /* 1 stop bit */
    mode |= ZYNQ_UART_MODE_NBSTOP(ZYNQ_UART_MODE_NBSTOP_STOP_1);
  }

  regs->control &= ~(ZYNQ_UART_CONTROL_RXEN | ZYNQ_UART_CONTROL_TXEN);
  regs->mode = mode;
  /* Ignore baud rate of B0. There are no modem control lines to de-assert */
  if (baud > 0) {
    regs->baud_rate_gen = ZYNQ_UART_BAUD_RATE_GEN_CD(brgr);
    regs->baud_rate_div = ZYNQ_UART_BAUD_RATE_DIV_BDIV(bauddiv);
    regs->control |= ZYNQ_UART_CONTROL_RXRES
      | ZYNQ_UART_CONTROL_TXRES;
  }
  regs->control |= ZYNQ_UART_CONTROL_RXEN | ZYNQ_UART_CONTROL_TXEN;

  /*
   * Some ZynqMP UARTs have a hardware bug that causes TX/RX logic restarts to
   * require a kick after baud rate registers are initialized.
   */
  zynq_uart_write_polled(context, 0);

  return true;
}

const rtems_termios_device_handler zynq_uart_handler = {
  .first_open = zynq_uart_first_open,
  .set_attributes = zynq_uart_set_attributes,
  .write = zynq_uart_write_support,
#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS
  .last_close = zynq_uart_last_close,
  .mode = TERMIOS_IRQ_DRIVEN
#else
  .poll_read = zynq_uart_read_polled,
  .mode = TERMIOS_POLLED
#endif
};
