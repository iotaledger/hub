DELIMITER $$

CREATE TRIGGER hub_address_balance_insert 
AFTER INSERT ON hub_address_balance 
FOR EACH ROW 
BEGIN 
UPDATE hub_address 
SET balance = ( 
SELECT SUM(amount) FROM hub_address_balance 
WHERE hub_address = NEW.hub_address 
) 
WHERE id = NEW.hub_address; END $$

CREATE TRIGGER user_address_balance_insert 
AFTER INSERT ON user_address_balance 
FOR EACH ROW 
BEGIN 
UPDATE user_address 
SET balance = ( 
SELECT SUM(amount) FROM user_address_balance WHERE user_address = NEW.user_address
) 
WHERE id = NEW.user_address; END $$

CREATE TRIGGER user_account_balance_insert
AFTER INSERT ON user_account_balance
FOR EACH ROW
BEGIN
UPDATE user_account
SET balance = (
SELECT SUM(amount) FROM user_account_balance WHERE user_id = NEW.user_id
) WHERE id = NEW.user_id; END $$

CREATE TRIGGER sweep_tails_update 
AFTER UPDATE 
ON sweep_tails FOR EACH ROW 
BEGIN 
UPDATE sweep SET confirmed = NEW.confirmed 
WHERE sweep.id = NEW.sweep AND sweep.confirmed = 0; END $$

DELIMITER ;
