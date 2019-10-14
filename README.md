# crackzippassword
修改7-zip源码(19.00)支持暴力破解zip压缩文件
你需要下载7z源码对照修改编译，增加了-pf命令，-pfdict.txt的dict.txt会先去读取dict.txt，如果没有则把"dict.txt"当作密码每一位可能出现的字符.Release提供的是windows x86的执行文件，其他平台请自己编译
