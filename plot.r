d1 = read.csv("cpp/data_cpp_filt.txt", col.names=c("raw","smth"))
plot(d1$raw, ty='l')
lines(d1$smth, col=2)
