CREATE TABLE `users` ( /* info about each user account including their address and balance */
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `pubkey` char(130) DEFAULT NULL, /* used as login identifier */
  `address` char(40) DEFAULT NULL, /* unique address generated for user to make pmts into */
  `refund` char(40) DEFAULT NULL,  /* user provided refund address if needed */
  `balance` decimal(13,0) DEFAULT NULL,  /* current balance of unspent pmts */
  `created` datetime DEFAULT NULL, /* when the user created account */
  `ts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
)

CREATE TABLE `pmts` ( /* payment audit history with various details for each payment detected */
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `uid` int(11) DEFAULT NULL, 		/* link to users.id for user who made pmt */
  `trxhash` char(64) DEFAULT NULL,	/* the transaction hash for pmt */
  `blockidx` int(11) DEFAULT NULL, 	/* the block height that includes the pmt */
  `amount` decimal(13,0) DEFAULT NULL,	/* value of pmt made */
  `received` datetime DEFAULT NULL,	/* time pmt was detected */
  PRIMARY KEY (`id`)
)

CREATE TABLE `jobs` (  /* info about pending jobs that may be run when a user has sufficient balance */
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `uid` int(11) DEFAULT NULL,		/* user who owns job */
  `status` tinyint(4) DEFAULT '0',	/* job status eg. pending, paid, completed etc. */
  `fee` decimal(9,0) DEFAULT '0',	/* fee for job */
  /* more job control fields here */
  PRIMARY KEY (`id`)
)
