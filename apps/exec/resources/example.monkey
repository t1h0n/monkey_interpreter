let map = fn(arr, f) {
    let iter = fn(arr, accumulated) {
        if (len(arr) == 0) {
            accumulated
        } else {
            iter(rest(arr), push(accumulated, f(first(arr))));
        }
    };
    iter(arr, []);
};
let a = [1, 2, 3, 4];
let double = fn(x) { x * 2 };
puts("original: ", a, "mapped: ", map(a, double));

let reduce = fn(arr, initial, f) {
    let iter = fn(arr, result) {
        if (len(arr) == 0) {
            result
        } else {
            iter(rest(arr), f(result, first(arr)));
        }
    };
    iter(arr, initial);
};
let sum = fn(arr) {
    reduce(arr, 0, fn(initial, el) { initial + el });
};

puts("original: ", a, "summed: ", sum(a));

let mp = {"foo":"b" + "a" + "r", "thr" + "ee": 6 / 2, true : false};
let mp_ = push(mp, false, sum(a));
let mp_f = erase(mp, "foo");
puts("mp before push:", mp, "mp after push:", mp_, "mp after erasing foo", mp_f);

let i = 1;
while(i < 4)
{
    puts("i is now:", i);
    let i = i + 1; 
}