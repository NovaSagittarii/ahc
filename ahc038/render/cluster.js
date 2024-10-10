function randint(lo, hi) {
    return floor(random(lo, hi+1));
}
function fx(x) {
    return 200+x*12;
}
function f(x, y) {
    ellipse(fx(x), fx(y), 5, 5);
}
function Node(x, y) {
    this.x = x;
    this.y = y;
    this.match = false;
}

randomSeed(0);
var bn = 125;
var CMIN = -10;
var CMAX = 10;
var FORCE_PARITY = true;
var b = [];
for (var i = 0; i < bn; ++i) {
    while (true) {
        var x = randint(CMIN, CMAX);
        var y = randint(CMIN, CMAX);
        if (FORCE_PARITY && (x + y) % 2 === 0) { // force parity
            y += 2*randint(0,1)-1;
        }
        var ok = true;
        for (var j = 0; j < b.length; ++j) {
            if (b[j].x === x && b[j].y === y) {
                ok = false;
                break;
            }
        }
        if (ok) {
            b.push(new Node(x, y));
            break;
        }
    }
}

noStroke();
fill(255, 0, 0);
f(0, 0);
fill(255);
stroke(0);
for (var i = 0; i < b.length; ++i) { f(b[i].x, b[i].y); }

var L = [null, 3, 0, 0];
var Lmax = L.length-1;

function matches(cx, cy, x, y, dry) {
    for (var i = 0; i < bn; ++i) {
        // if (b[i].x === cx && b[i].y === cy) { return false; }
        if (!b[i].match && b[i].x === x && b[i].y === y) {
            if (!dry) { 
                stroke(0);
                line(fx(x), fx(y), fx(cx), fx(cy));
                noStroke();
                ellipse(fx(x), fx(y), 5, 5);
                b[i].match = true;
            }
            return true;
        }
    }
    return false;
}
function cluster(cx, cy, dry) {
    var tot = 0;
    fill(random(100, 255), random(100, 255), random(100, 255));
    for (var d = 1; d <= Lmax; ++d) {
        var w = L[d];
        if (w) { w -= matches(cx, cy, cx, cy-d, dry); }
        if (w) { w -= matches(cx, cy, cx-d, cy, dry); }
        if (w) { w -= matches(cx, cy, cx+d, cy, dry); }
        if (w) { w -= matches(cx, cy, cx, cy+d, dry); }
        tot += L[d] - w;
    }
    return tot;
}

var cluster_count = 0;
function randomGreedy() {
    var remain = bn;
    for (var iter = 0; iter < 1000 && remain; ++iter) {
        // random greedy
        var cx = randint(CMIN, CMAX);
        var cy = randint(CMIN, CMAX);
        var cx = (iter % (CMAX - CMIN + 1)) + CMIN;
        // var cy = floor(iter / (CMAX - CMIN + 1)) + CMIN;
        var used = cluster(cx, cy) > 0;
        cluster_count += used;
    }
}
function greedy1() {
    var a = [];
    for (var i = 0; i < b.length; ++i) {
        a.push([(b[i].y*1e2+b[i].x+1e5).toString().padStart(10, '0'), null, b[i]]);
    }
    a.sort();
    function connect(cx, cy) {
        fill(random(0, 255), random(0, 255), random(0, 255));
        var w = L[1];
        if (w) { w -= matches(cx, cy, cx, cy-1); }
        if (w) { w -= matches(cx, cy, cx-1, cy); }
        if (w) { w -= matches(cx, cy, cx+1, cy); }
        if (w) { w -= matches(cx, cy, cx, cy+1); }
        return w !== L[1];
    }
    for (var i = 0; i < a.length; ++i) {
        if (i < 10) {
            fill(255, 0, 0);
            text(i, fx(a[i][2].x), fx(a[i][2].y));
        }
        if (a[i][2].match) { continue; }
        if (i+1 < a.length && a[i][2].y === a[i+1][2].y && a[i][2].x + 2 === a[i+1][2].x && !a[i+1][2].match) {
            cluster_count += connect(a[i][2].x+1, a[i][2].y);
        } else {
            cluster_count += connect(a[i][2].x, a[i][2].y+1);
        }
    }
}
function greedyK() {
    var a = [];
    for (var i = 0; i < b.length; ++i) {
        a.push([(b[i].y*1e2+b[i].x+1e5).toString().padStart(10, '0'), b[i]]);
    }
    a.sort();
    
    for (var i = 0; i < a.length; ++i) {
        var p = a[i][1];
        var best = 1;
        var best_qual = 1;
        for (var d = Lmax; d >= 1; --d) {
            var qual = cluster(p.x+d, p.y, true);
            if (qual > best_qual) {
                best_qual = qual;
                best = d;
            }
        }
        if (best_qual > 1) {
            cluster_count += cluster(p.x + best, p.y) > 0;
        } else {
            for (var d = Lmax; d >= 1; --d) {
                var qual = cluster(p.x, p.y+d, true);
                if (qual > best_qual) {
                    best_qual = qual;
                    best = d;
                }
            }
            cluster_count += cluster(p.x, p.y + best) > 0;
        }
    }
}
// randomGreedy();
greedy1();
// greedyK();

fill(0);
text("clusters: " + cluster_count, 20, 20);

enableContextMenu();
