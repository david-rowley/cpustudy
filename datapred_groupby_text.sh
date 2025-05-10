#!/bin/bash

dbname=postgres
psql -c "drop table if exists a;" $dbname
psql -c "create table a (a text);" $dbname
psql -c "alter system set work_mem = '4GB';" $dbname
psql -c "alter system set jit = 0;" $dbname
psql -c "alter system set max_parallel_workers_per_gather = 0;" $dbname
psql -c "select pg_reload_conf();" $dbname
psql -c "insert into a select md5(a::text) from generate_series(1,960) a;" $dbname

echo "select a from a group by a;" > bench.sql

for i in {1..14}
do
	psql -c "vacuum freeze a;" $dbname > /dev/null
	psql -c "select pg_prewarm('a');" $dbname > /dev/null
	psql -t -c "select pg_relation_size('a')/1024;" $dbname
	#psql -c "explain select a from a group by a;" $dbname
	pgbench -n -f bench.sql -t 10 $dbname | grep latency
	
	psql -c "insert into a with maxa as (select count(*) a from a) select md5(g.s::text) from maxa, lateral generate_Series(maxa.a+1, maxa.a*2) g(s);" $dbname > /dev/null
done
psql -c "vacuum freeze a;" $dbname > /dev/null
psql -c "select pg_prewarm('a');" $dbname > /dev/null
psql -t -c "select pg_relation_size('a')/1024;" $dbname
#psql -c "explain select a from a group by a;" $dbname
pgbench -n -f bench.sql -t 10 $dbname | grep latency
