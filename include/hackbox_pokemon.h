#pragma once

#define	PMC_INCDEC		(0)
#define	PMC_INIT		(0)
#define	PMC_INC			(1)
#define	PMC_DEC			(2)

/**
 * @brief 宝可梦生成参数定义
 * 定义用于生成宝可梦的各种属性参数索引
 */
#define PMAKE_NAME           ( 0 )   // 宝可梦编号(16位)
#define PMAKE_LEVEL          ( 1 )   // 等级(8位)
#define PMAKE_EXP            ( 2 )   // 经验值(32位)
#define PMAKE_ID             ( 3 )   // ID号(32位)
#define PMAKE_PERRND         ( 4 )   // 个体值随机数(32位)
#define PMAKE_SEX            ( 5 )   // 性别(8位)
#define PMAKE_PERSONAL       ( 6 )   // 性格(8位)
#define PMAKE_WAZA1          ( 7 )   // 持有技能1(8位)
#define PMAKE_WAZA2          ( 8 )   // 持有技能2(8位)
#define PMAKE_WAZA3          ( 9 )   // 持有技能3(8位)
#define PMAKE_WAZA4          ( 10 )  // 持有技能4(8位)
#define PMAKE_ITEM           ( 11 )  // 携带物品(8位)
#define PMAKE_SPABI          ( 12 )  // 特殊能力(1位)
#define PMAKE_HP_RND         ( 13 )  // HP个体值(5位)
#define PMAKE_HP_EXP         ( 14 )  // HP努力值(8位)
#define PMAKE_POW_RND        ( 15 )  // 攻击个体值(5位)
#define PMAKE_POW_EXP        ( 16 )  // 攻击努力值(8位)
#define PMAKE_DEF_RND        ( 17 )  // 防御个体值(5位)
#define PMAKE_DEF_EXP        ( 18 )  // 防御努力值(8位)
#define PMAKE_AGI_RND        ( 19 )  // 速度个体值(5位)
#define PMAKE_AGI_EXP        ( 20 )  // 速度努力值(8位)
#define PMAKE_EXPOW_RND      ( 21 )  // 特攻个体值(5位)
#define PMAKE_EXPOW_EXP      ( 22 )  // 特攻努力值(8位)
#define PMAKE_EXDEF_RND      ( 23 )  // 特防个体值(5位)
#define PMAKE_EXDEF_EXP      ( 24 )  // 特防努力值(8位)
#define PMAKE_STYLE          ( 25 )  // 帅气度(8位)
#define PMAKE_BEAUTIFUL      ( 26 )  // 美丽度(8位)
#define PMAKE_CUTE           ( 27 )  // 可爱度(8位)
#define PMAKE_CLEVER         ( 28 )  // 聪明度(8位)
#define PMAKE_STRONG         ( 29 )  // 强壮度(8位)
#define PMAKE_FUR            ( 30 )  // 毛发光泽度(8位)
#define PMAKE_FRIEND         ( 31 )  // 亲密度(8位)
#define PMAKE_POKERUS        ( 32 )  // 病毒状态(8位)
#define PMAKE_TAMAGO         ( 33 )  // 蛋状态标志(1位)
#define PMAKE_CONDITION      ( 34 )  // 身体状况(32位)
#define PMAKE_HP             ( 35 )  // HP当前值
#define PMAKE_POW            ( 36 )  // 攻击力
#define PMAKE_DEF            ( 37 )  // 防御力
#define PMAKE_AGI            ( 38 )  // 速度
#define PMAKE_EXPOW          ( 39 )  // 特攻
#define PMAKE_EXDEF          ( 40 )  // 特防
#define PMAKE_EVGET          ( 41 )  // 事件分发标志(1位)
#define PMAKE_GETPLACE       ( 42 )  // 捕获地点1(10位)
#define PMAKE_GETLEVEL       ( 43 )  // 捕获等级(7位)
#define PMAKE_GETCASSETTE    ( 44 )  // 捕获卡带(4位)
#define PMAKE_GETBALL        ( 45 )  // 捕获球种(8位)
#define PMAKE_CONTRY_CODE    ( 46 )  // 国家代码(8位)
#define PMAKE_FORM_NO        ( 47 )  // 形态编号(8位)
#define PMAKE_GETPLACE_Y     ( 48 )  // 捕获地点1年份
#define PMAKE_GETPLACE_M     ( 49 )  // 捕获地点1月份
#define PMAKE_GETPLACE_D     ( 50 )  // 捕获地点1日期
#define PMAKE_GETPLACE2      ( 51 )  // 捕获地点2(10位)
#define PMAKE_GETPLACE2_Y    ( 52 )  // 捕获地点2年份
#define PMAKE_GETPLACE2_M    ( 53 )  // 捕获地点2月份
#define PMAKE_GETPLACE2_D    ( 54 )  // 捕获地点2日期
#define PMAKE_NICKNAME       ( 55 )  // 昵称标志

#define PMAKE_BATTLEPARAM1	( 0xfe )
#define PMAKE_BATTLEPARAM2	( 0xfd )

void DebugPokemonMakeInit(HackBoxTool *hackBox);