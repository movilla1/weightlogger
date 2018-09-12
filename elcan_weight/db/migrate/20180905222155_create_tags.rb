class CreateTags < ActiveRecord::Migration
  def change
    create_table :tags do |t|
      t.string :tag_id
      t.references :user, index: true, foreign_key: true
      t.boolean :active

      t.timestamps null: false
    end
  end
end
