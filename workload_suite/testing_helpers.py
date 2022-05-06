def build_command(b, n, x, e, s, d, a, k):
    command = "./buffermanager -b {} -n {} -x {} -e {} " \
              "-s {} -d {} -a {} -k {}".format(b, n, x, e, s, d, a, k)
    return command


def test_algo(b, n, x, e, s, d, k, algo):
    command = "./buffermanager -b {} -n {} -x {} -e {} " \
              "-s {} -d {} -a {} -k {}".format(b, n, x, e, s, d, k, algo)
    return command


def file_name(b, n, x, e, s, d, a, k, extension):
    file_path = "workload_suite/raw_results/buffersize{}_disksize{}_numops{}_" \
               "rw{}_skewedperct{}_skeweddataperct{}_algorithm{}_pagesize{}" \
               ".{}".format(b, n, x, e, s, d, a, k,  extension)
    return file_path


def discriminate(filename):
    if int(get_params(filename, "b")) > int(int(get_params(filename, "n"))/10):
        return False
    if get_params(filename, "e") != "1090" and get_params(filename, "e") != "5050" and get_params(filename, "e") != "1000":
        return False
    if int(get_params(filename, "k")) <= 10:
        return False
    if int(get_params(filename, "k")) != 4096:
        return False
    return True

def discriminate_skew(filename):
    if  get_params(filename, "s") != "70.000000" and get_params(filename, "s") != "30.000000" and get_params(filename, "s") != "50.000000" and get_params(filename, "s") != "100.000000" and get_params(filename, "s") != "10.000000":
        return False
    return True

def get_params(filename, param):
    if(param == "b"):
        return exclude_before_after(filename, "b-", "_n-")
    if(param == "n"):
        return exclude_before_after(filename, "n-", "_x-")
    if(param == "x"):
        return exclude_before_after(filename, "x-", "_e-")
    if(param == "e"):
        return exclude_before_after(filename, "e-", "_s-")
    if(param == "s"):
        return exclude_before_after(filename, "s-", "d-")
    if(param == "d"):
        return exclude_before_after(filename, "d-", "_v-")
    if(param == "a"):
        return exclude_before_after(filename, "a-", "k-")
    if(param == "k"):
        return exclude_before_after(filename, "k-", ".txt")
    return


def exclude_before_after(filename, before, after):
    removed_after = filename[:filename.index(after)]
    removed_before = removed_after[removed_after.index(before)+ len(before):]
    return removed_before
