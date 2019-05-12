-- We're not normalising seeds into a table of their own on purpose.

CREATE TABLE IF NOT EXISTS user_account (
  id INTEGER PRIMARY KEY /*!40101 AUTO_INCREMENT */,
  balance BIGINT DEFAULT 0 NOT NULL,
  identifier VARCHAR(64) NOT NULL UNIQUE
);

CREATE INDEX idx_user_account_identifier ON user_account(identifier);
CREATE INDEX idx_user_account_balance ON user_account(balance);

CREATE TABLE IF NOT EXISTS user_address (
  id INTEGER PRIMARY KEY /*!40101 AUTO_INCREMENT */,
  address CHAR(81) NOT NULL,
  user_id INTEGER NOT NULL,
  seed_uuid CHAR(64) NOT NULL,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
  balance BIGINT DEFAULT 0 NOT NULL,
  FOREIGN KEY (user_id) REFERENCES user_account(id)
);

CREATE INDEX idx_user_address_address ON user_address(address);
CREATE INDEX idx_user_address_balance ON user_address(balance);

CREATE TABLE IF NOT EXISTS hub_address (
  id INTEGER PRIMARY KEY /*!40101 AUTO_INCREMENT */,
  address CHAR(81) NOT NULL,
  seed_uuid CHAR(64) NOT NULL,
  is_cold_storage INTEGER DEFAULT 0 NOT NULL,
  balance BIGINT DEFAULT 0 NOT NULL,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL
);


CREATE INDEX idx_hub_address_address ON hub_address(address);
CREATE INDEX idx_hub_address_balance ON hub_address(balance);

CREATE TABLE IF NOT EXISTS sweep (
  id INTEGER PRIMARY KEY /*!40101 AUTO_INCREMENT */,
  bundle_hash CHAR(81) NOT NULL UNIQUE,
  trytes LONGTEXT NOT NULL,
  into_hub_address INTEGER NOT NULL,
  confirmed INTEGER DEFAULT 0 NOT NULL,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
  FOREIGN KEY (into_hub_address) REFERENCES hub_address(id)
);

-- reason: 0 INBOUND 1 OUTBOUND
CREATE TABLE IF NOT EXISTS hub_address_balance (
  id INTEGER PRIMARY KEY /*!40101 AUTO_INCREMENT */,
  hub_address INTEGER NOT NULL,
  amount BIGINT NOT NULL,
  reason INTEGER NOT NULL,
  sweep INTEGER NOT NULL,
  occured_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  CONSTRAINT hadd_reason_amount CHECK ((reason = 0 and amount >= 0) or (reason = 1 and amount < 0)),
  CONSTRAINT hadd_reason_range CHECK ((reason = 0 or reason = 1)),
  FOREIGN KEY (hub_address) REFERENCES hub_address(id),
  FOREIGN KEY (sweep) REFERENCES sweep(id)
);


CREATE INDEX idx_hub_address_balance_hub_address ON hub_address_balance(hub_address);
CREATE INDEX idx_hub_address_balance_reason ON hub_address_balance(hub_address, reason);

-- reason: 0 DEPOSIT 1 SWEEP
-- if user_address_balance is NE 0 for a given user_account, then this user_account's funds have not been swept yet.
CREATE TABLE IF NOT EXISTS user_address_balance (
  id INTEGER PRIMARY KEY /*!40101 AUTO_INCREMENT */,
  user_address INTEGER NOT NULL,
  amount BIGINT NOT NULL,
  reason INTEGER NOT NULL,
  -- confirmed tail's hash
  tail_hash CHAR(81) DEFAULT NULL,
  -- nullable if not swept yet
  sweep INTEGER DEFAULT NULL,
  message TEXT(2187) DEFAULT NULL,
  occured_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  CONSTRAINT uab_reason_amount CHECK ((reason = 0 and tail_hash is not null and amount > 0) or (reason = 1 and sweep is not null and amount < 0)),
  CONSTRAINT uab_reason_range CHECK ((reason = 0 or reason = 1)),
  FOREIGN KEY (user_address) REFERENCES user_address(id),
  FOREIGN KEY (sweep) REFERENCES sweep(id)
);

CREATE INDEX idx_user_address_balance_reason ON user_address_balance(user_address, reason);
CREATE INDEX idx_user_address_occuredAt ON user_address_balance(occured_at);

CREATE TABLE IF NOT EXISTS withdrawal (
  id INTEGER PRIMARY KEY /*!40101 AUTO_INCREMENT */,
  uuid CHAR(36) UNIQUE NOT NULL,
  user_id INTEGER NOT NULL,
  amount BIGINT NOT NULL,
  -- payout address
  payout_address CHAR(81) NOT NULL,
  tag CHAR(27) DEFAULT NULL,
  -- sweep that processes this withdrawal
  sweep INTEGER DEFAULT NULL,
  requested_at DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
  cancelled_at DATETIME DEFAULT NULL,
  CONSTRAINT amount CHECK ((amount > 0)),
  FOREIGN KEY (sweep) REFERENCES sweep(id),
  FOREIGN KEY (user_id) REFERENCES user_account(id)
);

CREATE INDEX idx_withdrawal_uuid ON withdrawal(uuid);
CREATE INDEX idx_withdrawal_cancelled_at ON withdrawal(cancelled_at);
CREATE INDEX idx_withdrawal_by_request_ts ON withdrawal(requested_at ASC);

-- reason: 0 SWEEP 1 BUY 2 WITHDRAW_CANCEL 3 WITHDRAW 4 SELL
CREATE TABLE IF NOT EXISTS user_account_balance (
  user_id INTEGER NOT NULL,
  amount BIGINT NOT NULL,
  reason INTEGER NOT NULL,
  sweep INTEGER,
  withdrawal INTEGER,
  occured_at DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
  CONSTRAINT uacc_reason_amount CHECK (
    (reason = 0 and sweep is not null and amount > 0) or
    (reason = 1 and amount > 0) or
    (reason = 2 and withdrawal is not null and amount > 0) or
    (reason = 3 and withdrawal is not null and amount < 0) or
    (reason = 4 and amount < 0)
  ),
  CONSTRAINT uacc_reason_range CHECK ((reason >= 0 and reason < 5)),
  FOREIGN KEY (user_id) REFERENCES user_account(id),
  FOREIGN KEY (sweep) REFERENCES sweep(id),
  FOREIGN KEY (withdrawal) REFERENCES withdrawal(id)
);


CREATE INDEX idx_user_account_balance_by_user_id ON user_account_balance(user_id);

CREATE TABLE IF NOT EXISTS sweep_tails (
  hash CHAR(81) PRIMARY KEY NOT NULL,
  sweep INTEGER NOT NULL,
  confirmed INTEGER NOT NULL DEFAULT 0,
  created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (sweep) REFERENCES sweep(id)
);

CREATE INDEX idx_pending_tails_by_sweep ON sweep_tails(sweep);

CREATE TABLE IF NOT EXISTS signed_uuids(
  uuid CHAR(64) PRIMARY KEY NOT NULL UNIQUE
);

