PORT        ?= /dev/ttyUSB0
BAUD        ?= 115200
NUM_TRACES  ?= 1000

.PHONY: capture-traces
capture-traces:
	$(shell sed -i 's/^capture_trace.*/''capture_traces $(NUM_TRACES) $(subst /,\/,$(PRF_HOME)/work/traces.trs)''/g' $(BSX_HOME)/flow/acquisition/gather-traces.txt)
	$(SASS_RIG)/shell.py \
        --port $(PORT) \
        --baud $(BAUD) \
        --source $(BSX_HOME)/flow/acquisition/gather-traces.txt

.PHONY: capture-t-func-ttest
capture-t-func-ttest:
	$(SASS_RIG)/shell.py \
        --port $(PORT) \
        --baud $(BAUD) \
        --source gather-t-func-ttest.txt

.PHONY: t-func-verify
t-func-verify:
	$(SASS_RIG)/shell.py \
        --port $(PORT) \
        --baud $(BAUD) \
        --source t-func-verify.txt

t-func-ttest-eval:
	$(SASS_RIG)/shell.py --source ttest-eval.txt
