/**
 * https://en.wikipedia.org/wiki/Simulated_annealing
 * https://en.wikipedia.org/wiki/2-opt
 **/

// parameters
var N     = 50;
var kMax  = 100000;
var kStep = 1000;
var kMaxTemp = 1e-1;
var kMinTemp = 1e-9;

var kHistory = 30;

var CMIN = -10;
var CMAX = 10;
var FORCE_PARITY = false;
var L = [null, 1, 1, 1];
var Lmax = L.length-1;

randomSeed(0);
// setup
enableContextMenu();
var a = [];
var sol = [];
var score_history = [];


function randint(lo, hi) {
    return floor(random(lo, hi+1));
}
for (var i = 0; i < N; ++i) {
    while (true) {
        var x = randint(CMIN, CMAX);
        var y = randint(CMIN, CMAX);
        if (FORCE_PARITY && (x + y) % 2 === 0) { // force parity
            y += 2*randint(0,1)-1;
        }
        var ok = true;
        for (var j = 0; j < a.length; ++j) {
            if (a[j][0] === x && a[j][1] === y) {
                ok = false;
                break;
            }
        }
        if (ok) {
            a.push([x, y]);
            sol.push([1, 0, 1]);
            break;
        }
    }
}

for (var i = 0; i < kHistory; ++i) {
    score_history.push([1e9, 0]);
}



function Temperature(t) {
    return ((Math.exp(t) - 1) / (Math.E - 1)) * (kMaxTemp - kMinTemp) + kMinTemp;
}

function GenerateNewSolution(sol) {
    var n = sol.length;
    var ret = sol.slice(0); // make a copy
    
    var d = randint(1, Lmax);
    var rng = randint(0, 3);
    var vals = [0,1,0,-1,0];
    var dx = vals[rng] * d;
    var dy = vals[rng+1] * d;
    ret[randint(0, n-1)] = [dx, dy, d];
    
    return ret;
}

function Evaluate(sol) {
    var pairs = {};
    var cost = 0;
    for (var i = 0; i < N; ++i) {
        var x = a[i][0];
        var y = a[i][1];
        var cx = x + sol[i][0];
        var cy = y + sol[i][1];
        var k = cx + "," + cy;
        if (!pairs[k]) {
            pairs[k] = [0,0,0,0,0,0,0];
        }
        ++pairs[k][sol[i][2]];
    }
    for (var k in pairs) {
        var tmp = 1;
        for (var d = 1; d <= Lmax; ++d) {
            tmp = max(tmp, ceil(pairs[k][d] / L[d]));
        }
        cost += tmp;
    }
    return cost;
}

function P(sol, newsol, T) { // return accept probability
    var e = Evaluate(sol);
    var ep = Evaluate(newsol);
    if (ep < e) {
        return 1;
    } else {
        return Math.exp(-(ep - e) / T);
    }
}

var T = -1;
var k = 0;
var score = 0;

function fx(x) {
    return 200+12*x;
}
var draw = function() {
    // render
    background(255);
    stroke(0, 80);
    noFill();
    for (var i = 0; i < N; ++i) {
        ellipse(fx(a[i][0]), fx(a[i][1]), 5, 5);
        line(fx(a[i][0]), fx(a[i][1]), fx(a[i][0]+sol[i][0]), fx(a[i][1]+sol[i][1]));
    }
    // stroke(0);
    // line(a[sol[0]][0], a[sol[0]][1], a[sol[N-1]][0], a[sol[N-1]][1]);
    // for (var i = 1; i < N; ++i) {
    //     line(a[sol[i-1]][0], a[sol[i-1]][1], a[sol[i]][0], a[sol[i]][1]);
    // }
    
    score = Evaluate(sol);
    fill(0);
    text("iter = " + k + " (" + kMax + ")", 400, 15);
    text("Temperature = " + T.toFixed(2), 400, 30);
    text("Score = " + score.toFixed(2), 400, 45);
    text(this.__frameRate.toFixed(1) + "fps", 400, 380);
    
    noStroke();
    fill(0, 10);
    rect(400, 100, 200, 200);
    var slice = 200/kHistory;
    fill(0);
    for (var i = 0; i < kHistory; ++i) {
        var lo = Math.log(score_history[i][0])*50 - 0.5 - 400;
        var hi = Math.log(score_history[i][1])*50 + 0.5 - 400;
        rect(400 + i*slice, 100+200-hi, slice, hi-lo);
    }
    
    for (var i = 0; i < kStep; ++i) {
        // simulated annealing (one iteration)
        if (k < kMax) {
            T = Temperature(1 - (k+1) / kMax);
            // random solution
            var newsol = GenerateNewSolution(sol);
            if (P(sol, newsol, T) >= Math.random()) {
                sol = newsol;
            }
            
            ++k;
        } else {
            break;
        }
    }
    
    // update history data
    if (k < kMax) {
        var score = Evaluate(sol)*100;
        var j = Math.floor(kHistory * k / kMax);
        score_history[j][0] = Math.min(score_history[j][0], score);
        score_history[j][1] = Math.max(score_history[j][1], score);
    }
};
