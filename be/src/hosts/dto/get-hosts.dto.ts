import { Host } from '../entities/host.entity';

export class GetHostsResponseDto {
  version: number | null;
  result: Host[];
  httpStatusCode: number;
  message: string | null;
  isError: boolean;
}
