class CreateWeights < ActiveRecord::Migration
  def change
    create_table :weights do |t|
      t.float :weight
      t.references :truck, index: true, foreign_key: true
      t.integer :axis
      t.boolean :complete

      t.timestamps null: false
    end
  end
end
