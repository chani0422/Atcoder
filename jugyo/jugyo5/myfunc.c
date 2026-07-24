int kaijou(int n) {
    int ans = 1;
    for(int i = 1; i <= n; i++) {
        ans *= i;
    }
    return ans;
}

int C(int m, int n) {
    int ans = kaijou(m)/(kaijou(n)*kaijou(m-n));
    return ans;
}