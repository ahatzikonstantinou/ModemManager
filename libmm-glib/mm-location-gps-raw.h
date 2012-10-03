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
 * Copyright (C) 2012 Lanedo GmbH <aleksander@lanedo.com>
 */

#ifndef MM_LOCATION_GPS_RAW_H
#define MM_LOCATION_GPS_RAW_H

#if !defined (__LIBMM_GLIB_H_INSIDE__) && !defined (LIBMM_GLIB_COMPILATION)
#error "Only <libmm-glib.h> can be included directly."
#endif

#include <ModemManager.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define MM_TYPE_LOCATION_GPS_RAW            (mm_location_gps_raw_get_type ())
#define MM_LOCATION_GPS_RAW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MM_TYPE_LOCATION_GPS_RAW, MMLocationGpsRaw))
#define MM_LOCATION_GPS_RAW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  MM_TYPE_LOCATION_GPS_RAW, MMLocationGpsRawClass))
#define MM_IS_LOCATION_GPS_RAW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MM_TYPE_LOCATION_GPS_RAW))
#define MM_IS_LOCATION_GPS_RAW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  MM_TYPE_LOCATION_GPS_RAW))
#define MM_LOCATION_GPS_RAW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  MM_TYPE_LOCATION_GPS_RAW, MMLocationGpsRawClass))

/**
 * MM_LOCATION_GPS_RAW_LONGITUDE_UNKNOWN:
 *
 * Identifier for an unknown longitude value.
 *
 * Proper longitude values fall in the [-180,180] range.
 */
#define MM_LOCATION_GPS_RAW_LONGITUDE_UNKNOWN G_MINDOUBLE

/**
 * MM_LOCATION_GPS_RAW_LATITUDE_UNKNOWN:
 *
 * Identifier for an unknown latitude value.
 *
 * Proper latitude values fall in the [-90,90] range.
 */
#define MM_LOCATION_GPS_RAW_LATITUDE_UNKNOWN  G_MINDOUBLE

/**
 * MM_LOCATION_GPS_RAW_ALTITUDE_UNKNOWN:
 *
 * Identifier for an unknown altitude value.
 */
#define MM_LOCATION_GPS_RAW_ALTITUDE_UNKNOWN  G_MINDOUBLE

typedef struct _MMLocationGpsRaw MMLocationGpsRaw;
typedef struct _MMLocationGpsRawClass MMLocationGpsRawClass;
typedef struct _MMLocationGpsRawPrivate MMLocationGpsRawPrivate;

/**
 * MMLocationGpsRaw:
 *
 * The #MMLocationGpsRaw structure contains private data and should
 * only be accessed using the provided API.
 */
struct _MMLocationGpsRaw {
    /*< private >*/
    GObject parent;
    MMLocationGpsRawPrivate *priv;
};

struct _MMLocationGpsRawClass {
    /*< private >*/
    GObjectClass parent;
};

GType mm_location_gps_raw_get_type (void);

const gchar *mm_location_gps_raw_get_utc_time  (MMLocationGpsRaw *self);
gdouble      mm_location_gps_raw_get_longitude (MMLocationGpsRaw *self);
gdouble      mm_location_gps_raw_get_latitude  (MMLocationGpsRaw *self);
gdouble      mm_location_gps_raw_get_altitude  (MMLocationGpsRaw *self);

/*****************************************************************************/
/* ModemManager/libmm-glib/mmcli specific methods */

#if defined (_LIBMM_INSIDE_MM) ||    \
    defined (_LIBMM_INSIDE_MMCLI) || \
    defined (LIBMM_GLIB_COMPILATION)

MMLocationGpsRaw *mm_location_gps_raw_new (void);
MMLocationGpsRaw *mm_location_gps_raw_new_from_dictionary (GVariant *string,
                                                           GError **error);

gboolean mm_location_gps_raw_add_trace (MMLocationGpsRaw *self,
                                        const gchar *trace);

GVariant *mm_location_gps_raw_get_dictionary (MMLocationGpsRaw *self);

#endif

G_END_DECLS

#endif /* MM_LOCATION_GPS_RAW_H */