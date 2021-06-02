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
import constants

class Bit(object):
    def __init__(self, bit):
        self.bit = bit
        self.conn = None
    def setConn(self, connection):
        self.conn = connection

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
        self.conn = None
    def setConn(self, connection):
        self.conn = connection

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
        self.conn = None
    def setConn(self, connection):
        self.conn = connection

def adapt_text(text):
    if text.text is not None:
        t = adapt(text.text)
        t.prepare(text.conn)
        return AsIs("%s::text" % (t))
    else:
        return AsIs("NULL::text")

register_adapter(Text, adapt_text)

class BigInteger(object):
    def __init__(self, bigint):
        self.bigint = bigint
        self.conn = None
    def setConn(self, connection):
        self.conn = connection

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
        self.conn = None
    def setConn(self, connection):
        self.conn = connection

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
        self.conn = None
    def setConn(self, connection):
        self.conn = connection

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
        self.conn = None
    def setConn(self, connection):
        self.conn = connection

def adapt_array(array):
    if array.array is not None:
        #t = adapt(array.array)
        return AsIs("'{%s}'" % (array.array))
    else:
        return AsIs("NULL")

register_adapter(Array, adapt_array)

def execute(name, args):
    conn = None
    result = None
    try:
        conn = psycopg2.connect(constants.DB_CONNECTION_STRING)
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

def getSetting(name):
    return execute('get_setting', (Text(name),))[0][0]

def setSetting(name, value):
    return execute('set_setting', (Text(name), Text(value)))[0][0]

def getSource(identity):
    # destination, user_id, type_id, info
    return execute('get_source', (BigInteger(identity),))[0]

def getSourcesQueueSize():
    return execute('get_sources_queue_size', ())[0][0]

def addSource(destination, user_id=None, type_id=None, info=None):
    # source_id
    return execute('add_source', (Text(destination), BigInteger(user_id), BigInteger(type_id), Text(info)))[0][0]

def getTask():
    # destination, source_id, task_id, user_id, info
    return execute('get_task', ())[0]

def setTaskResult(task_id, json, status):
    return execute('set_task_result', (BigInteger(task_id), Text(json), Integer(status)))[0]

def getTaskResult(task_id):
    return execute('get_task_result', (BigInteger(task_id),))[0][0]

def getSourceResult(source_id):
    result = execute('get_source_result', (BigInteger(source_id),))
    # result, user_id
    return result[0] if result else (None, None)

def addMessage(message, type_id, user_id=None):
    # message_id
    return execute('add_message', (Text(message), BigInteger(type_id), BigInteger(user_id)))[0][0]

def checkLogin(name, password):
    identity = execute('check_login', (Text(name), Text(password)))
    return identity[0][0] if identity else None
