const int maxNum = 1005;
int shift[maxNum];
int Sunday(const string& T, const string& P) {
    int n = T.length();
    int m = P.length();

    // 默认值，移动m+1位
    for(int i = 0; i < maxNum; i++) {
        shift[i] = m + 1;
    }

    // 模式串P中每个字母出现的最后的下标
    // 所对应的主串参与匹配的最末位字符的下一位字符移动到该位，所需要的移动位数
    for(int i = 0; i < m; i++) {
        shift[P[i]] = m - i;
    }

    // 模式串开始位置在主串的哪里
    int s = 0;
    // 模式串已经匹配到的位置
    int j;
    while(s <= n - m) {
        j = 0;
        while(T[s + j] == P[j]) {
            j++;
            // 匹配成功
            if(j >= m) {
                return s;
            }
        }
        // 找到主串中当前跟模式串匹配的最末字符的下一个字符
        // 在模式串中出现最后的位置
        // 所需要从(模式串末尾+1)移动到该位置的步数
        s += shift[T[s + m]];
    }
    return -1;
}


int sunday_match(const char *src, const int src_len, const char *pattern, int pattern_len)
{
	int a = 0;
    int i = 0;
    int j = 0;
	int shift[256] = {0};
	
	//默认值，移动pattern_len+1位
	for (a=0; a<256; a++) {
		shift[a] = pattern_len + 1;
	}
	
	// 模式串P中每个字母出现的最后的下标
    // 所对应的主串参与匹配的最末位字符的下一位字符移动到该位，所需要的移动位数
    for(i = 0; i < pattern_len; i++) {
        shift[pattern[i]] = pattern_len - i;
    }
	
	i=0;
    while(i <= src_len - pattern_len) {
        j = 0;
        while(src[i + j] == pattern[j]) {
            j++;
            if(j >= pattern_len) {
                return i;
            }
        }
        // 找到主串中当前跟模式串匹配的最末字符的下一个字符
        // 在模式串中出现最后的位置
        // 所需要从(模式串末尾+1)移动到该位置的步数
        i += shift[src[i + pattern_len]];
    }
	
    return -1;
}



