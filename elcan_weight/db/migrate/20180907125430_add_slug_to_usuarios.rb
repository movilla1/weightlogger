class AddSlugToUsuarios < ActiveRecord::Migration
  def change
    add_column :usuarios, :slug, :string, unique: true
  end
end
