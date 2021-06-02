#!/bin/bash
pg_dump -h 127.0.0.1 -U postgres --schema-only fdf > fdf-schema.sql
