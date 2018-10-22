# frozen_string_literal: true

Rails.application.routes.draw do
  devise_for :users, ActiveAdmin::Devise.config
  ActiveAdmin.routes(self)
  root to: 'home#index'
  get '/about', to: 'home#about'
  get '/help', to: 'home#help'

  post '/weights/create_from_rfid', to: 'weights#create_from_rfid',
                                   as: :weights_create_from_rfid
  get 'reports/index'
  get 'reports/drivers'
  post 'reports/drivers'
  get 'reports/trucks'
  post 'reports/trucks'
end
