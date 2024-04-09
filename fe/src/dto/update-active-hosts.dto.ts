export interface UpdateActiveHostsPayload {
  number: number;
  name: string;
  state: number;
  iconId: number | null;
  isDeleted: boolean;
  modifiedById: number;
  modifiedTime: Date;
  createdById: number | null;
  createdTime: Date;
  id: number;
}
