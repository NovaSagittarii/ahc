import subprocess
subprocess.run(['make', 'build/jury'], stdout=subprocess.DEVNULL)

tot = 0
tot2 = 0
tests = 100


for i in range(tests):
    p = subprocess.Popen(f'./build/jury < ./in/{str(i).zfill(4)}.txt', shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    out, err = p.communicate('')
    score = int(err.split()[2])
    tot += score
    tot2 += score * score

Ex = tot / tests
Ex2 = tot2 / tests

mu = Ex
sigma = (Ex2 - Ex**2) ** 0.5
print(f"{mu:.2f} +/- {sigma:.2f}")

