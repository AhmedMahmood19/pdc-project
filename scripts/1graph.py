import matplotlib.pyplot as plt
import pandas as pd
df1=pd.read_csv("../graph/serial-log.csv")
df2=pd.read_csv("../graph/parallel-log.csv")
ax = df1.plot(x="Number of Records",y="Total Time (in s)",label="Serial Execution",color="dodgerblue",alpha=1,linewidth=2)
df2.plot(x="Number of Records",y="Total Time (in s)",label="Parallel Execution",color="darkorange",alpha=0.5,linewidth=5,ax=ax,figsize=(15,10),fontsize=14,title="Comparison of Serial and Parallel Execution")
plt.ylabel("Time (In Seconds)", fontsize=18)
plt.xlabel("Number of Records", fontsize=18)
ax.title.set_fontsize(26)       #fontsize of "Comparison..."
ax.legend(loc=2, prop={'size': 18}) #fontsize of legend "Serial Execution etc"
# ax.ticklabel_format(useMathText=True)   #replaces 1e9 with x10^9 
plt.savefig("../graph/generated-graph.png")
plt.show()