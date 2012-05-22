lookup_mx
---------

This is a tiny C program to perform a DNS-lookup for MX record.

To build and run tests:

    $ make
    $ make test_units

Usage:

    $ ./bin/lookup_mx github.com
    ALT2.ASPMX.L.GOOGLE.com

If there are more than one record for given domain name, **lookup_mx** outputs one with highest priority. If there are more than one record with highest priority, it picks one randomly.
