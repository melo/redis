#!/usr/bin/env perl

use strict;
use warnings;
use Redis;

my ($port) = @ARGV;
die "Usage: redis_publisher.pl port\n" unless $port;

my $redis = Redis->new( server => "127.0.0.1:$port", encoding => undef, reconnect => 24*60*60);

my $topic = 0;
while (1) {
  $redis->publish($topic, "\0msg $topic\0");
  $topic = ($topic + 1) % 8;
  sleep(1);
}
