# Copyright 2021 The Fraud Detection Framework Authors.  All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND< either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.

import psycopg2
from psycopg2.extensions import adapt, register_adapter, AsIs

DB_CONNECTION_STRING = 'postgresql://postgres:password@localhost:5432/fdf_statistics'

class Bit(object):
    def __init__(self, bit):
        self.bit = bit

def adapt_bit(bit):
    if bit.bit is not None:
        if bit.bit:
            return AsIs("1::bit")
        else:
            return AsIs("0::bit")
    else:
        return AsIs("NULL::bit")

register_adapter(Bit, adapt_bit)

class Timestamp(object):
    def __init__(self, timestamp):
        self.timestamp = timestamp

def adapt_timestamp(timestamp):
    if timestamp.timestamp is not None:
        t = adapt(timestamp.timestamp)
        return AsIs("%s::date" % (t))
    else:
        return AsIs("NULL::date")

register_adapter(Timestamp, adapt_timestamp)

class Text(object):
    def __init__(self, text):
        self.text = text

def adapt_text(text):
    if text.text is not None:
        t = adapt(text.text)
        return AsIs("%s::text" % (t))
    else:
        return AsIs("NULL::text")

register_adapter(Text, adapt_text)

class BigInteger(object):
    def __init__(self, bigint):
        self.bigint = bigint

def adapt_bigint(bigint):
    if bigint.bigint is not None:
        t = adapt(bigint.bigint)
        return AsIs("%s::bigint" % (t))
    else:
        return AsIs("NULL::bigint")

register_adapter(BigInteger, adapt_bigint)

class Integer(object):
    def __init__(self, integer):
        self.integer = integer

def adapt_integer(integer):
    if integer.integer is not None:
        t = adapt(integer.integer)
        return AsIs("%s::integer" % (t))
    else:
        return AsIs("NULL::integer")

register_adapter(Integer, adapt_integer)

class Double(object):
    def __init__(self, double):
        self.double = double

def adapt_double(double):
    if double.double is not None:
        t = adapt(double.double)
        return AsIs("%s::double precision" % (t))
    else:
        return AsIs("NULL::double precision")

register_adapter(Double, adapt_double)

class Array(object):
    def __init__(self, array):
        self.array = ",".join(array)

def adapt_array(array):
    if array.array is not None:
        #t = adapt(array.array)
        return AsIs("'{%s}'" % (array.array))
    else:
        return AsIs("NULL")

register_adapter(Array, adapt_array)

class Json(object):
    def __init__(self, jsn):
        self.json = jsn

def adapt_json(jsn):
    if jsn.json is not None:
        #t = adapt(jsn.json)
        return AsIs("'%s'::json" % (jsn.json))
    else:
        return AsIs("NULL::json")

register_adapter(Json, adapt_json)

def execute(name, args):
    conn = None
    result = None
    try:
        conn = psycopg2.connect(DB_CONNECTION_STRING)
        conn.set_client_encoding('UTF8')
        with conn:
            with conn.cursor() as cur:
                for arg in args:
                    arg.setConn(conn)
                cur.callproc(name, args)
                conn.commit()
                result = cur.fetchall()
    finally:
        if conn is not None:
            conn.close()
    return result

def insertSamples(samples):
    conn = None
    result = None
    try:
        conn = psycopg2.connect(DB_CONNECTION_STRING)
        conn.set_client_encoding('UTF8')
        with conn:
            with conn.cursor() as cur:
                query = "INSERT INTO deepfake {} values {}".format(
                    "(filename, augmentations, label, set, source_video, swapped_id, target_id, is_original)", 
                    ','.join(['%s'] * len(samples)))
                cur.execute(query, samples)
                conn.commit()
    finally:
        if conn is not None:
            conn.close()
