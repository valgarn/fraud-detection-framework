#!/bin/bash
pg_dump -h 127.0.0.1 -U postgres -Fc fdf > fdf.dmp
