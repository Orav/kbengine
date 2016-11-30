# -*- coding: utf-8 -*-

"""
Django settings for KBEWebConsole project.

Generated by 'django-admin startproject' using Django 1.8.9.

For more information on this file, see
https://docs.djangoproject.com/en/1.8/topics/settings/

For the full list of settings and their values, see
https://docs.djangoproject.com/en/1.8/ref/settings/
"""

# Build paths inside the project like this: os.path.join(BASE_DIR, ...)
import os
import django

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


# Quick-start development settings - unsuitable for production
# See https://docs.djangoproject.com/en/1.8/howto/deployment/checklist/

# SECURITY WARNING: keep the secret key used in production secret!
SECRET_KEY = '(z-^8#+9$zm1fz^k7c-g!$%8d62d3jaomcl4f05%4kxqj71o2_'

# SECURITY WARNING: don't run with debug turned on in production!
DEBUG = True

ALLOWED_HOSTS = ["*"]

# Application definition

INSTALLED_APPS = (
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    
    'WebConsole',
)

if django.VERSION < (1, 7):
	MIDDLEWARE_CLASSES = (
	    'django.contrib.sessions.middleware.SessionMiddleware',
	    'django.middleware.common.CommonMiddleware',
	    #'django.middleware.csrf.CsrfViewMiddleware',
	    'django.contrib.auth.middleware.AuthenticationMiddleware',
	    'django.contrib.messages.middleware.MessageMiddleware',
	    'django.middleware.clickjacking.XFrameOptionsMiddleware',
	)

if django.VERSION >= (1, 8):
	MIDDLEWARE_CLASSES = (
	    'django.contrib.sessions.middleware.SessionMiddleware',
	    'django.middleware.common.CommonMiddleware',
	    #'django.middleware.csrf.CsrfViewMiddleware',
	    'django.contrib.auth.middleware.AuthenticationMiddleware',
	    'django.contrib.auth.middleware.SessionAuthenticationMiddleware',
	    'django.contrib.messages.middleware.MessageMiddleware',
	    'django.middleware.clickjacking.XFrameOptionsMiddleware',
	    'django.middleware.security.SecurityMiddleware',
	)

ROOT_URLCONF = 'KBESettings.urls'

TEMPLATES = [
    {
        'BACKEND': 'django.template.backends.django.DjangoTemplates',
        # 模版文件的存放路径，可以有多个
        'DIRS': [
            BASE_DIR + "/templates"
            ],
        'APP_DIRS': True,
        'OPTIONS': {
            'context_processors': [
                'django.template.context_processors.debug',
                'django.template.context_processors.request',
                'django.contrib.auth.context_processors.auth',
                'django.contrib.messages.context_processors.messages',
            ],
        },
    },
]

WSGI_APPLICATION = 'KBESettings.wsgi.application'


# Database
# https://docs.djangoproject.com/en/1.8/ref/settings/#databases

DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.sqlite3',
        'NAME': os.path.join(BASE_DIR, 'db.sqlite3'),
    }
}


# Internationalization
# https://docs.djangoproject.com/en/1.8/topics/i18n/

#LANGUAGE_CODE = 'en-us'
LANGUAGE_CODE = 'zh-hans'

#TIME_ZONE = 'UTC'
TIME_ZONE = 'Asia/Shanghai'

USE_I18N = True

USE_L10N = True

USE_TZ = True

DATETIME_FORMAT = '%Y-%m-%d %H:%M:%S'


# Static files (CSS, JavaScript, Images)
# https://docs.djangoproject.com/en/1.8/howto/static-files/

STATIC_URL = '/static/'

# 静态文件的存放路径，可以有多个
STATICFILES_DIRS  = (
    os.path.join(BASE_DIR, "static"),
)

if django.VERSION < (1, 7):
	# for Django-1.6.11
	TEMPLATE_DIRS = ( BASE_DIR + "/templates", )
	TEMPLATE_DEBUG = True
	TEMPLATE_CONTEXT_PROCESSORS = (
	    'django.core.context_processors.request',
	)

# BOOL
# 是否使用Machines的缓冲机制；
# 使用些机制可以减缓页面打开时的卡顿现象，
# 但对于实时性来说，会有些偏差
# 注意：此机制只能在独立运行时设置成true，
#       如果部署到apache等http服务器上以后此参数仍然为True，
#       有可能会造成大量的udp广播包
USE_MACHINES_BUFFER = True

# FLOAT；单位：秒
# 当多长时间没有查询Machines相关数据时停止缓冲机制，
# 以避免长时间没有人操作还不停的广播查询包
STOP_BUFFER_TIME = 300

# FLOAT；单位：秒
# Machines缓冲机制的刷新间隔
# 间隔时间越短，越精确，发送的数据也越频繁
# 此参数仅在 USE_MACHINES_BUFFER = True 时起作用
MACHINES_BUFFER_FLUSH_TIME = 1.0

# FLOAT；单位：秒
# 每次查询Machine时等待响应的最长时间
MACHINES_QUERY_WAIT_TIME = 1.0

# MACHINES地址配置
# 当此参数不为空时，则由原来的广播探测改为固定地址探测，
# WebConsole仅会以此配置的地址进行探测，
# 当服务器存在跨网段的情况时，此方案犹为有用。
# 例子：
# MACHINES_ADDRESS = ["192.168.0.1", "10.0.0.1", "172.16.0.1"]
MACHINES_ADDRESS = []

