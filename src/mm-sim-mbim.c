/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details:
 *
 * Copyright (C) 2013 Aleksander Morgado <aleksander@gnu.org>
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <ModemManager.h>
#define _LIBMM_INSIDE_MM
#include <libmm-glib.h>

#include "mm-error-helpers.h"
#include "mm-log.h"
#include "mm-sim-mbim.h"

G_DEFINE_TYPE (MMSimMbim, mm_sim_mbim, MM_TYPE_SIM)

/*****************************************************************************/

static gboolean
peek_device (gpointer self,
             MbimDevice **o_device,
             GAsyncReadyCallback callback,
             gpointer user_data)
{
    MMBaseModem *modem = NULL;
    MMMbimPort *port;

    g_object_get (G_OBJECT (self),
                  MM_SIM_MODEM, &modem,
                  NULL);
    g_assert (MM_IS_BASE_MODEM (modem));

    port = mm_base_modem_peek_port_mbim (modem);
    g_object_unref (modem);

    if (!port) {
        g_simple_async_report_error_in_idle (G_OBJECT (self),
                                             callback,
                                             user_data,
                                             MM_CORE_ERROR,
                                             MM_CORE_ERROR_FAILED,
                                             "Couldn't peek MBIM port");
        return FALSE;
    }

    *o_device = mm_mbim_port_peek_device (port);
    return TRUE;
}

/*****************************************************************************/
/* Send PIN */

static gboolean
send_pin_finish (MMSim *self,
                 GAsyncResult *res,
                 GError **error)
{
    return !g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error);
}

static void
pin_set_enter_ready (MbimDevice *device,
                     GAsyncResult *res,
                     GSimpleAsyncResult *simple)
{
    GError *error = NULL;
    MbimMessage *response;
    MbimPinType pin_type;
    MbimPinState pin_state;

    response = mbim_device_command_finish (device, res, &error);
    if (response &&
        mbim_message_basic_connect_pin_set_response_parse (
            response,
            &pin_type,
            &pin_state,
            NULL,
            &error)) {
        /* Create the errors ourselves */
        if (pin_type == MBIM_PIN_TYPE_PIN1 && pin_state == MBIM_PIN_STATE_LOCKED)
            error = mm_mobile_equipment_error_for_code (MM_MOBILE_EQUIPMENT_ERROR_INCORRECT_PASSWORD);
        else if (pin_type == MBIM_PIN_TYPE_PUK1 && pin_state == MBIM_PIN_STATE_LOCKED)
            error = mm_mobile_equipment_error_for_code (MM_MOBILE_EQUIPMENT_ERROR_SIM_PUK);
    }

    if (error)
        g_simple_async_result_take_error (simple, error);
    else
        g_simple_async_result_set_op_res_gboolean (simple, TRUE);
    g_simple_async_result_complete (simple);
    g_object_unref (simple);
    if (response)
        mbim_message_unref (response);
}

static void
send_pin (MMSim *self,
          const gchar *pin,
          GAsyncReadyCallback callback,
          gpointer user_data)
{
    MbimDevice *device;
    MbimMessage *message;
    GSimpleAsyncResult *result;
    GError *error = NULL;

    if (!peek_device (self, &device, callback, user_data))
        return;

    result = g_simple_async_result_new (G_OBJECT (self), callback, user_data, send_pin);

    mm_dbg ("Sending PIN...");
    message = (mbim_message_basic_connect_pin_set_request_new (
                   mbim_device_get_next_transaction_id (device),
                   MBIM_PIN_TYPE_PIN1,
                   MBIM_PIN_OPERATION_ENTER,
                   pin,
                   "",
                   &error));
    if (!message) {
        g_simple_async_result_take_error (result, error);
        g_simple_async_result_complete_in_idle (result);
        g_object_unref (result);
        return;
    }

    mbim_device_command (device,
                         message,
                         10,
                         NULL,
                         (GAsyncReadyCallback)pin_set_enter_ready,
                         result);
    mbim_message_unref (message);
}

/*****************************************************************************/

MMSim *
mm_sim_mbim_new_finish (GAsyncResult  *res,
                        GError       **error)
{
    GObject *source;
    GObject *sim;

    source = g_async_result_get_source_object (res);
    sim = g_async_initable_new_finish (G_ASYNC_INITABLE (source), res, error);
    g_object_unref (source);

    if (!sim)
        return NULL;

    /* Only export valid SIMs */
    mm_sim_export (MM_SIM (sim));

    return MM_SIM (sim);
}

void
mm_sim_mbim_new (MMBaseModem *modem,
                 GCancellable *cancellable,
                 GAsyncReadyCallback callback,
                 gpointer user_data)
{
    g_async_initable_new_async (MM_TYPE_SIM_MBIM,
                                G_PRIORITY_DEFAULT,
                                cancellable,
                                callback,
                                user_data,
                                MM_SIM_MODEM, modem,
                                NULL);
}

static void
mm_sim_mbim_init (MMSimMbim *self)
{
}

static void
mm_sim_mbim_class_init (MMSimMbimClass *klass)
{
    MMSimClass *sim_class = MM_SIM_CLASS (klass);

    sim_class->send_pin = send_pin;
    sim_class->send_pin_finish = send_pin_finish;
}