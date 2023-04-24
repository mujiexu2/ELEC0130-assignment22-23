import pandas as pd
import numpy as np

# 设置随机种子，保证每次运行生成的随机数据都一样
np.random.seed(123)

# 创建日期范围为2022年1月1日至2022年12月31日的索引
date_range = pd.date_range(start='2022-01-01', end='2022-12-31', freq='D')

# 生成随机的商品销量数据
data = np.random.randint(low=50, high=200, size=(len(date_range), 3))
df = pd.DataFrame(data=data, index=date_range, columns=['Milk', 'Cheese', 'Coke'])

# 添加一定的周期性
df['Milk'] += np.sin(np.arange(len(df)) * 2 * int(np.pi / 30)) * 50  # 一个月的周期
df['Cheese'] += np.sin(np.arange(len(df)) * 2 * int(np.pi / 7)) * 20  # 一周的周期
df['Coke'] += np.sin(np.arange(len(df)) * 2 * int(np.pi / 365)) * 80  # 一年的周期

# 查看数据
df = df.rename_axis('date')
print(df.head())
df.to_csv('saledata.csv')
