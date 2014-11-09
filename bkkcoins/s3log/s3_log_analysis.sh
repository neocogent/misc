#!/bin/bash

S3_LOG_LOCATION="s3://thelogbucket/logdir"
LOCAL_LOG_LOCATION="/tmp/log/"
REPORT_TITLE="My report title"
HTML_OUTPUT_DIR="/tmp/reporthtml"
REPORT="/tmp/report.ps"

# Sync log files to a local directory
s3cmd -v sync ${S3_LOG_LOCATION} ${LOCAL_LOG_LOCATION}

# Cat all files. Optional filtering can be done as well. Rewrite as Apache-like logs in sorted format. Push through webalizer
cat ${LOCAL_LOG_LOCATION}/* | awk -F" " '{ ORS=" "; print $5,"-","-",$3,$4,$10,$11,$12,$13,$15,"-"; for (i=20; i<NF; i++) print $(i); print "\n"; }' | sed 's/^ //g' | sort -t ' ' -k 4.9,4.12n -k 4.5,4.7M -k 4.2,4.3n -k 4.14,4.15n -k 4.17,4.18n -k 4.20,4.21n | webalizer -d -i -G -H -o ${HTML_OUTPUT_DIR} -n "${REPORT_TITLE}" - 

# Convert HTML report to postscript
html2ps --web b -r -b ${HTML_OUTPUT_DIR}/index.html > ${REPORT}

echo "Wrote ${REPORT}"
