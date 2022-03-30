def build_command(b, n, x, e, s, d, a):
    command = "./../buffermanager -b {} -n {} -x {} -e {} " \
              "-s {} -d {} -a {} > {}".format(b, n, x, e, s, d, a, 
		      file_name(b, n, x, e, s, d, a, "txt"))
    return command


def test_algo(b, n, x, e, s, d, algo):
    command = "./../buffermanager -b {} -n {} -x {} -e {} " \
              "-s {} -d {} -a {}".format(b, n, x, e, s, d, algo)
    return command


def file_name(b, n, x, e, s, d, a, extension):
    file_path = "raw_results/buffersize_{}_disksize_{}_numops_{}_" \
               "rw_{}_skewedperct_{}_skeweddataperct{}_algorithm_{}" \
               ".{}".format(b, n, x, e, s, d, a, extension)
    return file_path
