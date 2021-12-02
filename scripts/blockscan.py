#
# Bitcoin rewards script
#

import os, sys, json, datetime
from bitcoinrpc.authproxy import AuthServiceProxy, JSONRPCException
from decimal import Decimal

cfg = { 'rpc':'http://user:pwd@127.0.0.1:8332' }

rpc = AuthServiceProxy(cfg['rpc'], timeout=240)

nBlk = 0
blkmax = rpc.getblockcount()
rewards = Decimal(0)
unspent = Decimal(0)
fees = Decimal(0)

while nBlk < blkmax:
	blkhash = rpc.getblockhash(nBlk)
	blkdata = rpc.getblock(blkhash, 2)
	reward = Decimal(0)
	fee = Decimal(0)
	for tx in blkdata['tx']:
		if 'coinbase' in tx['vin'][0]:
			for vout in tx['vout']:
				reward += vout['value']
				utxo = rpc.gettxout(tx['txid'], vout['n'])
				if utxo is None:
					unspent += vout['value']
		if 'fee' in tx:
			fee += tx['fee']
	reward = max(0,reward-fee)
	rewards += reward
	fees += fee
	if not reward == Decimal((int(5e9) >> (nBlk // 210000))/1e8):
		print('Block:',nBlk,'Reward:',reward, flush=True)
	elif nBlk % 50000 == 0:
		print('Block:', nBlk, datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"), rewards, unspent, fees, flush=True)
	nBlk += 1
	
print('\nDone', datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
print('\nBlocks:', nBlk)   
print('Total rewards:', rewards)
print('Total unspent:', unspent)
print('Total fees:', fees) 
