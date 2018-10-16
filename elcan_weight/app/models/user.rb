# frozen_string_literal: true

class User < ActiveRecord::Base
  # Include default devise modules. Others available are:
  # :confirmable, :lockable, :timeoutable and :omniauthable
  devise :database_authenticatable, :recoverable, :rememberable,
         :trackable, :validatable
  extend FriendlyId
  has_many :trucks
  has_many :tags

  friendly_id :username, use: :slugged

  def find_with_tag(tag_id)
    tag = tags.find_by(tag_id: tag_id)
    tag.user if tag.present?
  end
end
