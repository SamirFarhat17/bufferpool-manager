def build_command(b, n, x, e, s, d, a):
    command = "./../buffermanager -b {} -n {} -x {} -e {} " \
              "-s {} -d {} -a {}".format(b, n, x, e, s, d, a)
    return command


def test_algo(b, n, x, e, s, d, algo):
    command = "./../buffermanager -b {} -n {} -x {} -e {} " \
              "-s {} -d {} -a {}".format(b, n, x, e, s, d, algo)
    return command


def write_file(b, n, x, e, s, d, a):
    filepath = "raw_results/buffersize_{}_disksize_{}_numops_{}_" \
               "rw_{}_skewedperct_{}_skeweddataperct{}_algorithm_{}" \
               ".json".format(b, n, x, e, s, d, a)
