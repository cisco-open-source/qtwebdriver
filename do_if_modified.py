import subprocess as sp
import os
import sys

if (len(sys.argv) != 4):
    print 'Wrong usage:'
    print ' do_if_modified <bin_file> <input_file> <output_file>'
    exit()

action_process = os.path.abspath(sys.argv[1])
input_file = os.path.abspath(sys.argv[2])
output_file = os.path.abspath(sys.argv[3])

need_update = True

if (os.path.exists(output_file)):

    input_mtime = os.path.getmtime(input_file)
    output_mtime = os.path.getmtime(output_file)

    if (input_mtime < output_mtime):
        need_update = False

if need_update:
    p = sp.Popen([action_process, input_file , "-o", output_file], stdin=sp.PIPE, stdout=sp.PIPE)
