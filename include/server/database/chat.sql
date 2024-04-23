SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

DROP TABLE IF EXISTS `User`;
CREATE TABLE `User`  (
  `id` INT NOT NULL AUTO_INCREMENT COMMENT '用户id',
  `name` varchar(50) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '用户名',
  `password` varchar(50) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '用户密码',
  `state` ENUM('online','offline') NOT NULL DEFAULT 'offline' COMMENT '当前登录状态',
  `last_online_time` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

  PRIMARY KEY (`id`),
  UNIQUE KEY(`name`)
  
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

DROP TABLE IF EXISTS `Friend`;
CREATE TABLE `Friend`  (
  `userid` INT NOT NULL COMMENT '用户ID',
  `friendid` INT NOT NULL COMMENT '好友ID',

  PRIMARY KEY(`userid`,`friendid`)
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

DROP TABLE IF EXISTS `AllGroup`;
CREATE TABLE `AllGroup`  (
  `id` INT NOT NULL AUTO_INCREMENT COMMENT '组ID' ,
  `groupname` varchar(50) NOT NULL COMMENT '组名称',
  `groupdesc` varchar(200) NOT NULL DEFAULT '' COMMENT '组功能描述',

  PRIMARY KEY(`id`)
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

DROP TABLE IF EXISTS `GroupUser`;
CREATE TABLE `GroupUser`  (
  `groupid` INT NOT NULL COMMENT '组ID' ,
  `userid` INT NOT NULL COMMENT '组成员id',
  `grouprole` ENUM('creator','normal') NOT NULL DEFAULT 'normal' COMMENT '组内角色',

  PRIMARY KEY(`groupid`)
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

DROP TABLE IF EXISTS `OfflineMessage`;
CREATE TABLE `OfflineMessage`  (
  `userid` INT NOT NULL COMMENT '用户ID' ,
  `message` varchar(500) NOT NULL COMMENT '离线消息' ,
  `create_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP

)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;


DROP TRIGGER IF EXISTS `USER_STATE_UPDATETIME`;
delimiter ;;
CREATE TRIGGER `USER_STATE_UPDATETIME` BEFORE UPDATE ON `User` FOR EACH ROW 
BEGIN
		IF NEW.state = "offline" THEN 
			SET NEW.last_online_time = CURRENT_TIMESTAMP;
		END IF;
END
;;
delimiter ;