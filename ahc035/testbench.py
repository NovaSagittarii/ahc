from multiprocessing import Pool
import subprocess
subprocess.run(['make', 'build/jury'], stdout=subprocess.DEVNULL)

tot = 0
tot2 = 0
tests = 100

def run_test(i, args=[]):
    p = subprocess.Popen(f'./build/jury {" ".join(args)} < ./in/{str(i).zfill(4)}.txt', shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    _out, err = p.communicate('')
    score = int(err.split()[2])
    return score

# for i in range(tests):
#     score = run_test(i)
#     tot += score
#     tot2 += score * score

# run in parallel
if __name__ == "__main__":
    with Pool() as pool: 
        w = pool.map(run_test, list(range(tests)))
        for score in w:
            tot += score
            tot2 += score * score

    Ex = tot / tests
    Ex2 = tot2 / tests

    mu = Ex
    sigma = (Ex2 - Ex**2) ** 0.5
    print(f"{mu:.2f} +/- {sigma:.2f}")

