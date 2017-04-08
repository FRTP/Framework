import requests
from datetime import timedelta
from datetime import datetime
from os import mkdir, chdir, path


def download_file(url, local_filename, proxies):
    r = requests.get(url, proxies=proxies, stream=True)
    with open(local_filename, 'wb') as f:
        for chunk in r.iter_content(chunk_size=1024):
            f.write(chunk)
    return local_filename


def download_info(share_name):
    proxies = {
      'http': 'http://92.242.44.43:8081'
    }
    delta = timedelta(days=1)
    cur_date = datetime.today() - delta
    end_date = datetime(year=2015, month=1, day=1)
    while cur_date > end_date:
        url = 'http://89.249.27.203:50011/papers/quik/tqbr/%s/%s/1000-1840/' \
              'm1/csv' % (share_name, cur_date.strftime('20%y%m%d'))
        local_filename = cur_date.strftime('20%y%m%d.csv')
        download_file(url, local_filename, proxies)
        cur_date -= delta


blue_chips = [
    'GAZP', 'SBER', 'LKOH', 'SNGS', 'SNGSP', 'MGNT', 'NVTK', 'GMKN', 'VTBR',
    'ROSN', 'TRNFP', 'TATN', 'MTSS', 'MOEX', 'ALRS', 'CHMF', 'SBERP', 'HYDR',
    'RSTI', 'AFLT', 'BANE', 'IRAO', 'MAGN', 'NLMK', 'FEES'
]
for share_name in blue_chips:
    if not path.exists(share_name):
        mkdir(share_name)
    chdir(share_name)
    download_info(share_name)
    chdir('..')
