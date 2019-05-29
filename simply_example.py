import json
import os
import time
import sys
import pandas as pd
import numpy as np
import psycopg2 as pg
from sklearn import metrics

from calendar import monthrange
from subprocess import call


def data_from_dwh():
    query = """
        SELECT se.*,
                ma.total_sales_number,
                ma.total_sales_volume,
                mc.lead_canal
                FROM stats.saex2 se
                JOIN brokerage_entity_map bem ON se.account_id = bem.brokerage_id
                JOIN stats.master_application ma ON bem.kredit_privat_id = ma.account_id 
                                                AND se.return_no = ma.return_id
                JOIN stats.la_marketing_canal mc ON ma.lead_la_id = mc.la_id 
                WHERE se.qualified_lead_date > '2018-12-31'
                AND se.qualified_lead_date < '2019-03-01'
        """
    conn = pg.connect(host='dwhdb.smava.de',
                      dbname='smava_stats',
                      user='reporting_r',
                      password='reporting_2015')

    cursor = conn.cursor()
    cursor.execute(query)
    df = pd.DataFrame(x for x in cursor)
    cols = [xs.name for xs in cursor.description]
    df.columns = cols
    return df


t0 = time.time()
df = data_from_dwh()
t1 = time.time()
df_bkup = df.copy()

df['sales'] = df['total_sales_volume'].fillna(0)
df['is_sale'] = df.sales > 0


# drop columns which are not required
# ==================================
def get_columns():
    cols = ["sales", "is_sale", "lead_canal"]
    xs = ['initial', 'first', 'second', 'third', 'fourth',
          'fifth', 'sixth', 'seventh', 'eight', 'ninth',
          'tenth']
    for x in xs:
        col1 = f"saex2_{x}_is_payout"
        col2 = f"saex2_{x}_actual"
        cols += [col1, col2]
    return cols

cols = get_columns()

df = df[cols].copy()

df.dropna(inplace=True)


def get_auc(df=df):
    xs = ['initial', 'first', 'second', 'third', 'fourth',
          'fifth', 'sixth', 'seventh', 'eight', 'ninth',
          'tenth']
    rslt = {}
    for x in xs:
        col = f"saex2_{x}_is_payout"
        try:
            auc = round(metrics.roc_auc_score(df["is_sale"], df[col]), 3)
        except:
            auc = None
        conversion = round(np.mean(df["is_sale"]), 3), round(np.mean(df[col]), 3)
        rslt[col] = auc, conversion
    return rslt


def get_mae(df=df):
    xs = ['initial', 'first', 'second', 'third', 'fourth',
          'fifth', 'sixth', 'seventh', 'eight', 'ninth',
          'tenth']
    rslt = {}
    for x in xs:
        col = f"saex2_{x}_actual"
        mae = round(np.mean(abs((df["sales"] - df[col]))), 3)
        conversion = 100 * abs(sum(df["sales"]) - sum(df[col])) / sum(df["sales"])
        rslt[col] = mae, round(conversion, 3)
    return rslt


t0 = time.time()
aucs = get_auc(df)
maes = get_mae(df)
t1 = time.time()

# channel level
auc_ch = {}
mae_ch = {}
for ch in set(df2.lead_canal):
    auc_ch[ch] = get_auc(df[df.lead_canal==ch])
    mae_ch[ch] = get_mae(df[df.lead_canal==ch])
