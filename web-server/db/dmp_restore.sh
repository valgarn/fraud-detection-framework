#!/bin/bash
pg_restore -d fdf -U postgres fdf_prod.dmp
