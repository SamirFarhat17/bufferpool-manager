def build_command(b, n, x, e, s, d, a, k):
    command = "./../buffermanager -b {} -n {} -x {} -e {} " \
              "-s {} -d {} -a {} -k {} > {}".format(b, n, x, e, s, d, a, 
		      file_name(b, n, x, e, s, d, a, "txt"))
    return command


def test_algo(b, n, x, e, s, d, k, algo):
    command = "./../buffermanager -b {} -n {} -x {} -e {} " \
              "-s {} -d {} -a {} -k {}".format(b, n, x, e, s, d, k, algo)
    return command


def file_name(b, n, x, e, s, d, a, k, extension):
    file_path = "raw_results/buffersize{}_disksize{}_numops{}_" \
               "rw{}_skewedperct{}_skeweddataperct{}_algorithm{}_pagesize{}" \
               ".{}".format(b, n, x, e, s, d, a, k,  extension)
    return file_path
