#!/bin/bash

dbname=postgres
psql -c "drop table if exists a;" $dbname > /dev/null
psql -c "create table a (a int);" $dbname > /dev/null
psql -c "alter system set jit = 0;" $dbname > /dev/null
psql -c "alter system set max_parallel_workers_per_gather = 0;" $dbname > /dev/null
psql -c "select pg_reload_conf();" $dbname > /dev/null

echo "select a from a group by a;" > bench.sql

for i in {0..14}
do
	if [ $i -eq 0 ]; then
		psql -c "insert into a select generate_series(1,1808);" $dbname > /dev/null
	else
		psql -c "insert into a with maxa as (select max(a) a from a) select g.s from maxa, lateral generate_Series(maxa.a+1, maxa.a*2) g(s);" $dbname > /dev/null
	fi
	
	psql -c "vacuum freeze a;" $dbname > /dev/null
	psql -c "select pg_prewarm('a');" $dbname > /dev/null
#	psql -c "explain select a from a group by a;" $dbname

	for work_mem in '32MB' '64MB' '128MB' '256MB' '512MB' '1GB' '2GB' '4GB'
	do
		psql -c "alter system set work_mem = '$work_mem';" $dbname > /dev/null
		psql -c "select pg_reload_conf();" $dbname > /dev/null
		echo -n "work_mem = $work_mem table_size = "
		psql -tA -F " " -c "select pg_size_pretty(pg_relation_size('a')),pg_relation_size('a')/1024;" $dbname | tr -d '\n'
		echo -n " "
		pgbench -n -f bench.sql -t 10 -M prepared $dbname | grep latency
	done
	
done
